#include "EngineInterface.h"
#include "LoadingDialog.h"
#include "IGameuiFuncs.h"

#include <vgui/IInput.h>
#include <vgui/ISurface.h>
#include <vgui/ILocalize.h>
#include <vgui/IScheme.h>
#include <vgui/ISystem.h>
#include <vgui_controls/ProgressBar.h>
#include <vgui_controls/Label.h>
#include <vgui_controls/Button.h>
#include <vgui_controls/HTML.h>
#include <vgui_controls/RichText.h>
#include "tier0/icommandline.h"

#include "GameUI_Interface.h"
#include "BasePanel.h"
#include "ModInfo.h"
#include "BitmapImagePanel.h"

using namespace vgui;

CBitmapImagePanel *CLoadingDialog::m_pLoadingBackground = NULL;

CLoadingDialog::CLoadingDialog(vgui::Panel *parent)
    : Frame(parent, "LoadingDialog")
{
	SetDeleteSelfOnClose(true);
	SetTitle("#GameUI_Loading", true);

	m_bCenter              = true;
	m_pParent              = parent;
	m_szBackgroundImage[0] = 0;

	m_bShowingSecondaryProgress         = false;
	m_flSecondaryProgress               = 0.0f;
	m_flLastSecondaryProgressUpdateTime = 0.0f;
	m_flSecondaryProgressStartTime      = 0.0f;

	m_pProgress           = new ProgressBar(this, "Progress");
	m_pProgress2          = new ProgressBar(this, "Progress2");
	m_pInfoLabel          = new Label(this, "InfoLabel", "");
	m_pCancelButton       = new Button(this, "CancelButton", "#GameUI_Cancel");
	m_pTimeRemainingLabel = new Label(this, "TimeRemainingLabel", "");
	m_pCancelButton->SetCommand("Cancel");

	if(!m_pLoadingBackground)
		m_pLoadingBackground = new CBitmapImagePanel(NULL, "Background");

	SetMinimizeButtonVisible(false);
	SetMaximizeButtonVisible(false);
	SetCloseButtonVisible(false);
	SetSizeable(false);
	SetMoveable(false);

	m_pInfoLabel->SetBounds(20, 32, 392, 24);
	m_pProgress->SetBounds(20, 64, 300, 24);
	m_pCancelButton->SetBounds(330, 64, 72, 24);
	m_pInfoLabel->SetTextColorState(Label::CS_DULL);
	m_pProgress2->SetVisible(false);

	SetupControlSettings(false);
}

CLoadingDialog::~CLoadingDialog(void)
{
}

void CLoadingDialog::PaintBackground(void)
{
	if(strlen(m_szBackgroundImage) > 0)
	{
		BaseClass::PaintBackground();
		return;
	}

	BaseClass::PaintBackground();
}

void CLoadingDialog::SetupControlSettings(bool bForceShowProgressText)
{
	LoadControlSettingsFromScheme("LoadingDialogNoBanner.res");
}

void CLoadingDialog::Open(bool bShowBackground)
{
	SetupControlSettings(false);

	m_bShowBackground = bShowBackground;

	if(bShowBackground)
	{
		int swide, stall;
		surface()->GetScreenSize(swide, stall);

		SetBounds(0, 0, swide, stall);
		SetTitle("#GameUI_Loading", true);
		SetPaintBackgroundEnabled(false);
		SetParent(m_pLoadingBackground);
		SetMouseInputEnabled(false);
		SetKeyBoardInputEnabled(false);

		m_pLoadingBackground->SetParent(m_pParent);
		m_pLoadingBackground->SetVisible(true);
		m_RestrictPanel = m_pLoadingBackground->GetVPanel();

		Activate();

		m_pProgress->SetVisible(true);

		if(!ModInfo().IsSinglePlayerOnly())
			m_pInfoLabel->SetVisible(true);

		m_pInfoLabel->SetText("");
		m_pCancelButton->SetText("#GameUI_Cancel");
		m_pCancelButton->SetCommand("Cancel");

		m_pInfoLabel->SetVisible(false);
		m_pCancelButton->SetVisible(false);
	}
	else
	{
		SetTitle("#GameUI_Loading", true);
		SetPaintBackgroundEnabled(true);
		SetParent(m_pParent);
		SetMouseInputEnabled(true);
		SetKeyBoardInputEnabled(true);

		m_RestrictPanel = GetVPanel();

		Activate();

		m_pLoadingBackground->SetVisible(false);
		m_pProgress->SetVisible(true);

		if(!ModInfo().IsSinglePlayerOnly())
			m_pInfoLabel->SetVisible(true);

		m_pInfoLabel->SetText("");
		m_pCancelButton->SetText("#GameUI_Cancel");
		m_pCancelButton->SetCommand("Cancel");

		m_pInfoLabel->SetVisible(true);
		m_pCancelButton->SetVisible(true);
	}
}

void CLoadingDialog::SetupControlSettingsForErrorDisplay(const char *settingsFile)
{
	m_bCenter = true;

	SetTitle("#GameUI_Disconnected", true);
	SetPaintBackgroundEnabled(true);
	SetParent(m_pParent);
	SetMouseInputEnabled(true);
	SetKeyBoardInputEnabled(true);

	m_pInfoLabel->SetText("");
	LoadControlSettingsFromScheme(settingsFile);

	m_RestrictPanel = GetVPanel();

	Activate();

	m_pLoadingBackground->SetVisible(false);
	m_pProgress->SetVisible(false);

	m_pInfoLabel->SetVisible(true);
	m_pCancelButton->SetText("#GameUI_Close");
	m_pCancelButton->SetCommand("Close");
	m_pInfoLabel->InvalidateLayout();
	m_pCancelButton->SetVisible(true);

	m_szBackgroundImage[0] = 0;
}

void CLoadingDialog::DisplayGenericError(const char *failureReason, const char *extendedReason)
{
	SetupControlSettingsForErrorDisplay("LoadingDialogError.res");

	if(extendedReason && strlen(extendedReason) > 0)
	{
		wchar_t compositeReason[256], finalMsg[512], formatStr[256];

		if(extendedReason[0] == '#')
			wcsncpy(compositeReason, g_pVGuiLocalize->Find(extendedReason), sizeof(compositeReason) / sizeof(wchar_t));
		else
			g_pVGuiLocalize->ConvertANSIToUnicode(extendedReason, compositeReason, sizeof(compositeReason));

		if(failureReason[0] == '#')
			wcsncpy(formatStr, g_pVGuiLocalize->Find(failureReason), sizeof(formatStr) / sizeof(wchar_t));
		else
			g_pVGuiLocalize->ConvertANSIToUnicode(failureReason, formatStr, sizeof(formatStr));

		g_pVGuiLocalize->ConstructString(finalMsg, sizeof(finalMsg), formatStr, 1, compositeReason);
		m_pInfoLabel->SetText(finalMsg);
	}
	else
		m_pInfoLabel->SetText(failureReason);
}

void CLoadingDialog::SetBackgroundImage(const char *imageName)
{
	if(!m_bShowBackground)
		return;

	if(strcmp(imageName, m_szBackgroundImage))
	{
		strcpy(m_szBackgroundImage, imageName);

		m_pLoadingBackground->setTexture(imageName);
	}
}

void CLoadingDialog::SetStatusText(const char *statusText)
{
	m_pInfoLabel->SetText(statusText);
}

void CLoadingDialog::OnThink(void)
{
	BaseClass::OnThink();

	if(m_bShowingSecondaryProgress)
	{
		wchar_t unicode[512];

		if(m_flSecondaryProgress >= 1.0f)
			m_pTimeRemainingLabel->SetText("complete");
		else if(ProgressBar::ConstructTimeRemainingString(unicode, sizeof(unicode), m_flSecondaryProgressStartTime, (float)system()->GetFrameTime(), m_flSecondaryProgress, m_flLastSecondaryProgressUpdateTime, true))
			m_pTimeRemainingLabel->SetText(unicode);
		else
			m_pTimeRemainingLabel->SetText("");
	}

	SetAlpha(255);
}

void CLoadingDialog::PerformLayout(void)
{
	int swide, stall;
	surface()->GetScreenSize(swide, stall);

	if(strlen(m_szBackgroundImage) > 0)
	{
		m_pLoadingBackground->SetBounds(0, 0, swide, stall);
		m_pLoadingBackground->SetBgColor(Color(0, 0, 0, 255));
	}

	if(m_bCenter)
	{
		MoveToCenterOfScreen();
	}
	else
	{
		int x, y, screenWide, screenTall;
		surface()->GetWorkspaceBounds(x, y, screenWide, screenTall);

		int wide, tall;
		GetSize(wide, tall);

		if(IsPC())
		{
			x = screenWide - (wide + 10);
			y = screenTall - (tall + 10);
		}
		else
		{
			x = screenWide - wide - (screenWide * 0.05);
			y = screenTall - tall - (screenTall * 0.05);
		}

		x -= m_iAdditionalIndentX;
		y -= m_iAdditionalIndentY;

		SetPos(x, y);
	}

	BaseClass::PerformLayout();

	if(m_bShowBackground)
	{
		int xofs = swide * 0.06;
		int tall = vgui::scheme()->GetProportionalScaledValue(18);
		m_pProgress->SetBounds(xofs, vgui::scheme()->GetProportionalScaledValue(436) + (tall / 2), swide * 0.88, tall);
	}

	vgui::ipanel()->MoveToFront(GetVPanel());
}

bool CLoadingDialog::SetProgressPoint(int progressPoint)
{
	int nOldDrawnSegments = m_pProgress->GetDrawnSegmentCount();
	m_pProgress->SetProgress((float)progressPoint / (m_iRangeMax - m_iRangeMin));
	int nNewDrawSegments = m_pProgress->GetDrawnSegmentCount();
	return (nOldDrawnSegments != nNewDrawSegments);
}

void CLoadingDialog::SetProgressRange(int min, int max)
{
	m_iRangeMin = min;
	m_iRangeMax = max;
}

void CLoadingDialog::SetSecondaryProgress(float progress)
{
	if(strlen(m_szBackgroundImage) > 0)
		return;

	if(!m_bShowingSecondaryProgress && progress > 0.99f)
		return;

	if(!m_bShowingSecondaryProgress)
	{
		LoadControlSettingsFromScheme("LoadingDialogDualProgress.res");
		m_bShowingSecondaryProgress = true;
		m_pProgress2->SetVisible(true);
		m_flSecondaryProgressStartTime = (float)system()->GetFrameTime();
	}

	if(progress > m_flSecondaryProgress)
	{
		m_pProgress2->SetProgress(progress);
		m_flSecondaryProgress               = progress;
		m_flLastSecondaryProgressUpdateTime = (float)system()->GetFrameTime();
	}

	if(progress < m_flSecondaryProgress)
	{
		m_pProgress2->SetProgress(progress);
		m_flSecondaryProgress               = progress;
		m_flLastSecondaryProgressUpdateTime = (float)system()->GetFrameTime();
		m_flSecondaryProgressStartTime      = (float)system()->GetFrameTime();
	}
}

void CLoadingDialog::SetSecondaryProgressText(const char *statusText)
{
	SetControlString("SecondaryProgressLabel", statusText);
}

void CLoadingDialog::OnClose(void)
{
	if(strlen(m_szBackgroundImage) > 0)
	{
		SetBackgroundImage("");

		m_pLoadingBackground->SetParent((VPANEL)NULL);
		m_pLoadingBackground->SetVisible(false);
	}

	m_RestrictPanel = NULL;
	vgui::surface()->RestrictPaintToSinglePanel(NULL);

	BaseClass::OnClose();
}

void CLoadingDialog::Activate(void)
{
	BaseClass::Activate();

	vgui::surface()->RestrictPaintToSinglePanel(GetVPanel());
}

void CLoadingDialog::OnCommand(const char *command)
{
	if(!stricmp(command, "Cancel"))
	{
		engine->pfnClientCmd("disconnect\n");

		Close();
	}
	else
		BaseClass::OnCommand(command);
}

void CLoadingDialog::OnKeyCodePressed(KeyCode code)
{
	if(code == KEY_ESCAPE)
		OnCommand("Cancel");
	else
		BaseClass::OnKeyCodePressed(code);
}

extern vgui::DHANDLE<CLoadingDialog> g_hLoadingDialog;

CLoadingDialog *LoadingDialog(void)
{
	return g_hLoadingDialog.Get();
}