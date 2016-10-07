
// WaveEditView.cpp : implementation of the CWaveEditView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "WaveEdit.h"
#endif

#include "WaveEditDoc.h"
#include "WaveEditView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CWaveEditView

IMPLEMENT_DYNCREATE(CWaveEditView, CScrollView)

BEGIN_MESSAGE_MAP(CWaveEditView, CScrollView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_COMMAND(ID_EDIT_CUT, &CWaveEditView::OnEditCut)
	ON_COMMAND(ID_EDIT_COPY, &CWaveEditView::OnEditCopy)
	ON_COMMAND(ID_EDIT_PASTE, &CWaveEditView::OnEditPaste)
	ON_COMMAND(ID_EDIT_UNDO32776, &CWaveEditView::OnEditUndo32776)
	ON_COMMAND(ID_EDIT_REDO32777, &CWaveEditView::OnEditRedo32777)
	ON_COMMAND(ID_EDIT_UNDO, &CWaveEditView::OnEditUndo)
	ON_COMMAND(ID_TOOLS_ECHO32778, &CWaveEditView::OnToolsEcho32778)
	ON_COMMAND(ID_TOOLS_SPEEDDOWN32779, &CWaveEditView::OnToolsSpeeddown32779)
	ON_COMMAND(ID_TOOLS_SPEEDUP32780, &CWaveEditView::OnToolsSpeedup32780)
	ON_COMMAND(ID_TOOLS_REVERSE, &CWaveEditView::OnToolsReverse)
	ON_COMMAND(ID_TOOLS_PLAYS, &CWaveEditView::OnToolsPlays)
	ON_COMMAND(ID_VIEW_ZOOMIN, &CWaveEditView::OnViewZoomin)
	ON_COMMAND(ID_VIEW_ZOOM, &CWaveEditView::OnViewZoom)
	ON_COMMAND(ID_VIEW_ZOOMOUT, &CWaveEditView::OnViewZoomout)
END_MESSAGE_MAP()

// CWaveEditView construction/destruction
double scfactor = 1;

int zin = 0;
int zout = 0;
int zid = 0;
int zod = 0;


std::stack<WaveFile *> undoS;
std::stack<WaveFile *> redoS;
CWaveEditView::CWaveEditView()
{
	mousePressed = false;
	selectionStart = 0;
	selectionEnd = 0;
	zoom = 1;
	// TODO: add construction code here

}

CWaveEditView::~CWaveEditView()
{
}

WaveFile * clipboard;

void
CWaveEditView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();
	// Initial scroll sizes
	CSize sizeTotal;
	sizeTotal.cx = 10000;
	sizeTotal.cy = 10000;
	SetScrollSizes(MM_TEXT, sizeTotal);
	
}

BOOL CWaveEditView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CWaveEditView drawing

void CWaveEditView::OnDraw(CDC* pDC)
{
	CWaveEditDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	//WaveFile * wave = &pDoc->wave;
	WaveFile * wave = (pDoc->wave);
	if (wave->hdr == NULL) {
		return;
	}
	// Get dimensions of the window.
	CRect rect;
	GetClientRect(rect);

	COLORREF color = RGB(255, 200, 200);
	CPen pen1(PS_SOLID, 0, color);
	pDC->SelectObject(&pen1);
	CBrush brush1(color);
	pDC->SelectObject(&brush1);
	// Draw selection if any
	if (selectionStart != selectionEnd) {
		pDC->Rectangle(selectionStart, 0, selectionEnd, rect.Height());
	}

	// Set color in pen and brush for wave
	color = RGB(0, 255, 0);
	CPen pen2(PS_SOLID, 0, color);
	pDC->SelectObject(&pen2);
	CBrush brush2(color);
	pDC->SelectObject(&brush2);
	// Draw the wave
	pDC->MoveTo(0, 0);
	int x;
	for (x = 0; x < rect.Width(); x++) {
		// Assuming the whole file will be fit in the window, for every x value in the window
		// we need to find the equivalent sample in the wave file.
		float val = wave->get_sample((int)(x*wave->lastSample / rect.Width()));
		// We need to fit the sound also in the y axis. The y axis goes from 0 in the
		// top of the window to rect.Height at the bottom. The sound goes from -32768 to 32767
		// we scale it in that way.
		int y = (int)((val + 32768) * (rect.Height() - 1) / (32767 + 32768));
		pDC->LineTo(x*scfactor, rect.Height() - y);
	}

	// TODO: add draw code for native data here
}


// CWaveEditView printing

BOOL CWaveEditView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CWaveEditView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CWaveEditView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// CWaveEditView diagnostics

#ifdef _DEBUG
void CWaveEditView::AssertValid() const
{
	CView::AssertValid();
}

void CWaveEditView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CWaveEditDoc* CWaveEditView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CWaveEditDoc)));
	return (CWaveEditDoc*)m_pDocument;
}
#endif //_DEBUG


// CWaveEditView message handlers


void CWaveEditView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	mousePressed = true;
	selectionStart = point.x;
	selectionEnd = point.x;

	CScrollView::OnLButtonDown(nFlags, point);
}


void CWaveEditView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	mousePressed = false;
	this->selectionEnd = point.x;
	this->RedrawWindow();
	CScrollView::OnLButtonUp(nFlags, point);
}


void CWaveEditView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CScrollView::OnMouseMove(nFlags, point);
	if (mousePressed) {
		selectionEnd = point.x;
		RedrawWindow();
	}
}


void CWaveEditView::OnEditCut()
{
	
	CWaveEditDoc* pDoc = GetDocument();
	

	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	//WaveFile * wave = &(pDoc->wave);
	WaveFile * wave = (pDoc->wave);
	if (wave->hdr == NULL) {
		return;
	}
	// Get dimensions of the window.
	CRect rect;
	GetClientRect(rect);
	// Scale the start and end of the selection.
	double startms = (1000.0 * wave->lastSample / wave->sampleRate) * this->selectionStart / rect.Width();
	// Scale the start and end of the selection.
	double endms = (1000.0 * wave->lastSample / wave->sampleRate) * this->selectionEnd / rect.Width();

	undoS.push((pDoc->wave));//added
	while (!redoS.empty())
	{
		redoS.pop();
	}
	//undoS.top()->play();
	clipboard = wave->getfragment(startms,endms);
	//clipboard->play();
	//undoS.push(wave);
//	delete &redoS; //deleting redoS
	//std::stack<WaveFile *> redoS; //redefining redoS

	WaveFile * w2 = wave->removefragment(startms, endms);


	
	//delete &redoS; //deleting redoS
	// Remove old wave
	//delete wave;
	// Substitute old wave with new one
	pDoc->wave = w2;//*
	// Update window
	wave->updateHeader();

	this->selectionEnd = 0;
	this->selectionStart = 0;

	this->RedrawWindow();


	// TODO: Add your command handler code here
}


void CWaveEditView::OnEditCopy()
{
	// TODO: Add your command handler code here
	CWaveEditDoc* pDoc = GetDocument();


	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	WaveFile * wave = pDoc->wave;//&
	if (wave->hdr == NULL) {
		return;
	}
	// Get dimensions of the window.
	CRect rect;
	GetClientRect(rect);
	// Scale the start and end of the selection.
	double startms = (1000.0 * wave->lastSample / wave->sampleRate) * this->selectionStart / rect.Width();
	// Scale the start and end of the selection.
	double endms = (1000.0 * wave->lastSample / wave->sampleRate) * this->selectionEnd / rect.Width();
	clipboard = wave->getfragment(startms, endms);
	clipboard->updateHeader();
	this->selectionEnd = 0;
	this->selectionStart = 0;
}


void CWaveEditView::OnEditPaste()
{
	// TODO: Add your command handler code here
	
	CWaveEditDoc* pDoc = GetDocument();


	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	WaveFile * wave = pDoc->wave; //&
	
	if (wave->hdr == NULL) {
		return;
	}
	 if (clipboard == NULL) {
		return;
	}
	// Get dimensions of the window.
	CRect rect;
	GetClientRect(rect);
	// Scale the start and end of the selection.
	double startms = (1000.0 * wave->lastSample / wave->sampleRate) * this->selectionStart / rect.Width();

	undoS.push((wave)); //added
	
	while (!redoS.empty())
	{
		redoS.pop();
	}
						
						
						//delete &redoS; //deleting redoS
	//std::stack<WaveFile *> redoS; //redefining redoS
	//undoS.push((pDoc->wave));//added
	
	//WaveFile * w2 = wave->insertFragment(clipboard, startms);
	
	pDoc->wave = wave->insertFragment(clipboard, startms);//*

	wave->updateHeader();

	this->selectionEnd = 0;
	this->selectionStart = 0;

	this->RedrawWindow();
	
/*
	CWaveEditDoc* pDoc = GetDocument();
	//CWaveEditApp* app = dynamic_cast<CWaveEditApp*>(AfxGetApp());

	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	WaveFile * wave = pDoc->wave;

	if (wave->hdr == NULL) {
		return;
	} if (clipboard == NULL) {
		return;
	}

	// Get dimensions of the window.
	CRect rect;
	GetClientRect(rect);

	//int width = (wave->maxSamples / wave->sampleRate) * scaleFactor; //Samplerate * (pixSecond/framerate)

																	 // Scale the start and end of the selection.
	//double startms = (1000.0 * wave->lastSample / wave->sampleRate) * this->selectionStart / width;
	double startms = (1000.0 * wave->lastSample / wave->sampleRate) * this->selectionStart / rect.Width();
	undoS.push((pDoc->wave));//added
	WaveFile * w2 = wave->insertFragment(clipboard, startms);

	pDoc->wave = w2;
	wave->updateHeader();
	this->selectionEnd = 0;
	this->selectionStart = 0;
	this->RedrawWindow();
	*/

}



void CWaveEditView::OnEditUndo32776()
{


}


void CWaveEditView::OnEditRedo32777()
{
	// TODO: Add your command handler code here
	if (redoS.empty())
	{
		return;
	}
	CWaveEditDoc* pDoc = GetDocument();


	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	WaveFile * wave = (pDoc->wave);//&
	if (wave->hdr == NULL) {
		return;
	}
	undoS.push((pDoc->wave));

	pDoc->wave = (redoS.top());
	redoS.pop();
	this->RedrawWindow();


}


void CWaveEditView::OnEditUndo()
{
	// TODO: Add your command handler code here
	if (undoS.empty())
	{
		return;
	}
	CWaveEditDoc* pDoc = GetDocument();


	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	WaveFile * wave = (pDoc->wave);//&
	if (wave->hdr == NULL) {
		return;
	}

	redoS.push((pDoc->wave));

	// TODO: Add your command handler code here
	//wave =
	//wave->play();
	//undoS.top()->play();
	//redoS.push(wave);
	
	//wave = undoS.top();

	//pDoc->wave = *wave;

	//CRect rect;
	//GetClientRect(rect);

	//double startms = (1000.0 * wave->lastSample / wave->sampleRate) * this->selectionStart / rect.Width();
	//WaveFile * w3 = wave->insertFragment(w2, startms);
	
	//wave->updateHeader();
	pDoc->wave =(undoS.top());
	undoS.pop();
	this->RedrawWindow();

}


void CWaveEditView::OnToolsEcho32778()
{
	CWaveEditDoc* pDoc = GetDocument();


	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	WaveFile * wave = (pDoc->wave);//&
	if (wave->hdr == NULL) {
		return;
	}
	CRect rect;
	GetClientRect(rect);
	double startms = (1000.0 * wave->lastSample / wave->sampleRate) * this->selectionStart / rect.Width();
	// Scale the start and end of the selection.
	double endms = (1000.0 * wave->lastSample / wave->sampleRate) * this->selectionEnd / rect.Width();

	if (startms == endms)
	{
		WaveFile * w2 = wave->echo(0.8, 100); //rem .
		undoS.push((pDoc->wave));//added


		while (!redoS.empty())
		{
			redoS.pop();
		}

		pDoc->wave = w2;
		w2->play();
	}
	else
	{
		WaveFile * edit = new WaveFile(wave->numChannels, wave->sampleRate, wave->bitsPerSample);
		edit = wave->getfragment(startms, endms);
		edit->updateHeader();
		edit = edit->echo(0.8, 100);
		edit->updateHeader();

		wave = wave->replaceFragment(edit,startms, endms);
		
	

		undoS.push((pDoc->wave));//added


		while (!redoS.empty())
		{
			redoS.pop();
		}


		//pDoc->wave = wave;
		wave->updateHeader();
		pDoc->wave = wave;
		wave->play();
	}

	// TODO: Add your command handler code here
	this->selectionEnd = 0;
	this->selectionStart = 0;
	//redoS.pop();
	this->RedrawWindow();

	
}


void CWaveEditView::OnToolsSpeeddown32779()
{
	// TODO: Add your command handler code here
	CWaveEditDoc* pDoc = GetDocument();


	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	WaveFile * wave = (pDoc->wave);//&
	if (wave->hdr == NULL) {
		return;
	}
	CRect rect;
	GetClientRect(rect);
	double startms = (1000.0 * wave->lastSample / wave->sampleRate) * this->selectionStart / rect.Width();
	// Scale the start and end of the selection.
	double endms = (1000.0 * wave->lastSample / wave->sampleRate) * this->selectionEnd / rect.Width();

	while (!redoS.empty())
	{
		redoS.pop();
	}


	if (startms == endms)
	{
		// TODO: Add your command handler code here
		WaveFile * w2 = wave->multiply_freq(0.5, 0); //rem .

		undoS.push((pDoc->wave));//added





		pDoc->wave = w2;
		//redoS.pop();
	}
	else
	{
		WaveFile * edit = new WaveFile(wave->numChannels, wave->sampleRate, wave->bitsPerSample);
		edit = wave->getfragment(startms, endms);
		edit = edit->multiply_freq(0.5,0);
		undoS.push((pDoc->wave));//added

		wave = wave->replaceFragment(edit,startms, endms);
		wave->updateHeader();
		pDoc->wave = wave;
	}
	
	this->selectionEnd = 0;
	this->selectionStart = 0;

	this->RedrawWindow();
	
	
	
	




}


void CWaveEditView::OnToolsSpeedup32780()
{
	// TODO: Add your command handler code here
	CWaveEditDoc* pDoc = GetDocument();


	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	WaveFile * wave = (pDoc->wave);//&
	if (wave->hdr == NULL) {
		return;
	}

	// TODO: Add your command handler code here
	CRect rect;
	GetClientRect(rect);
	double startms = (1000.0 * wave->lastSample / wave->sampleRate) * this->selectionStart / rect.Width();
	// Scale the start and end of the selection.
	double endms = (1000.0 * wave->lastSample / wave->sampleRate) * this->selectionEnd / rect.Width();

	while (!redoS.empty())
	{
		redoS.pop();
	}

	if (startms == endms)
	{
		// TODO: Add your command handler code here
		WaveFile * w2 = wave->multiply_freq(2, 0); //rem .

		undoS.push((pDoc->wave));//added
		pDoc->wave = w2;
		//redoS.pop();
	}
	else
	{
		WaveFile * edit = new WaveFile(wave->numChannels, wave->sampleRate, wave->bitsPerSample);
		edit = wave->getfragment(startms, endms);
		edit = edit->multiply_freq(2, 0);
		undoS.push((pDoc->wave));//added

		wave = wave->replaceFragment(edit, startms, endms);
		wave->updateHeader();
		pDoc->wave = wave;
	}

	this->selectionEnd = 0;
	this->selectionStart = 0;
	this->RedrawWindow();




	//w2->play();
}


void CWaveEditView::OnToolsReverse()
{
	// TODO: Add your command handler code here

	CWaveEditDoc* pDoc = GetDocument();


	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	WaveFile * wave = (pDoc->wave);//&
	if (wave->hdr == NULL) {
		return;
	}

	// TODO: Add your command handler code here
	CRect rect;
	GetClientRect(rect);
	double startms = (1000.0 * wave->lastSample / wave->sampleRate) * this->selectionStart / rect.Width();
	// Scale the start and end of the selection.
	double endms = (1000.0 * wave->lastSample / wave->sampleRate) * this->selectionEnd / rect.Width();


	while (!redoS.empty())
	{
		redoS.pop();
	}


	if (startms == endms)
	{
		undoS.push((pDoc->wave));//added

		wave = wave->reverseFragment();
		wave->updateHeader();
		pDoc->wave = wave;

	}
	else
	{
		WaveFile * nwave = new WaveFile(wave->numChannels, wave->sampleRate, wave->bitsPerSample);
		nwave = wave->getfragment(startms, endms);
		nwave->updateHeader();
		
		nwave = nwave->reverseFragment();
		nwave->updateHeader();
		undoS.push((pDoc->wave));//added





		wave = wave->replaceFragment(nwave,startms, endms);
		wave->updateHeader();
		pDoc->wave = wave;
		
	}

	this->RedrawWindow();
	this->selectionEnd = 0;
	this->selectionStart = 0;
}




void CWaveEditView::OnToolsPlays()
{
	// TODO: Add your command handler code here

	CWaveEditDoc* pDoc = GetDocument();


	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	WaveFile * wave = (pDoc->wave);//&
	if (wave->hdr == NULL) {
		return;
	}

	// TODO: Add your command handler code here
	CRect rect;
	GetClientRect(rect);
	double startms = (1000.0 * wave->lastSample / wave->sampleRate) * this->selectionStart / rect.Width();
	// Scale the start and end of the selection.
	double endms = (1000.0 * wave->lastSample / wave->sampleRate) * this->selectionEnd / rect.Width();

	if (startms  == endms)
	{
		wave->play();
	}
	else
	{
		WaveFile * w2 = wave->getfragment(startms, endms);
		w2->play();
	}
	this->selectionEnd = 0;
	this->selectionStart = 0;
}


void CWaveEditView::OnViewZoomin()
{
	// TODO: Add your command handler code here
	scfactor = scfactor*2;

	this->RedrawWindow();
}


void CWaveEditView::OnViewZoom()
{
	// TODO: Add your command handler code here
	
}


void CWaveEditView::OnViewZoomout()
{
	scfactor = scfactor/2;
	// TODO: Add your command handler code here
	this->RedrawWindow();

}
