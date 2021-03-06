#include "EngineInterface.h"
#include "OptionsSubAudio.h"

#include "CvarSlider.h"
#include "ModInfo.h"
#include "LabeledCommandComboBox.h"
#include "CvarToggleCheckButton.h"
#include "vgui_controls/ComboBox.h"
#include "vgui_controls/QueryBox.h"
#include "tier1/KeyValues.h"
#include <vgui/ISurface.h>
#include <vgui/IInput.h>
#include <tier1/strtools.h>

#include "CVARs.h"

using namespace vgui;

COptionsSubAudio::COptionsSubAudio(vgui::Panel *parent)
    : PropertyPage(parent, NULL)
{
	m_pEAXCheckButton = new CCvarToggleCheckButton(this, "EAX", "#GameUI_EnableEAX", "s_eax");
	m_pA3DCheckButton = new CCvarToggleCheckButton(this, "A3D", "#GameUI_EnableA3D", "s_a3d");

	m_pSFXSlider = new CCvarSlider(this, "SFXSlider", "#GameUI_SoundEffectVolume", 0.0f, 1.0f, "volume");
	m_pMusicSlider = new CCvarSlider(this, "MusicSlider", "#GameUI_MusicVolume", 0.0f, 1.0f, "mp3volume");

	m_pSoundQualityCombo = new CLabeledCommandComboBox(this, "SoundQuality");
	m_pSoundQualityCombo->AddItem("#GameUI_High", "hisound 1");
	m_pSoundQualityCombo->AddItem("#GameUI_Low", "hisound 0");
	m_pSoundQualityCombo->SetInitialItem(engine->pfnGetCvarFloat("hisound") != 0.0 ? 0 : 1);

	LoadControlSettingsFromScheme("OptionsSubAudio.res");

	if(!engine->pfnGetCvarPointer("s_eax"))
		m_pEAXCheckButton->SetVisible(false);

	if(!engine->pfnGetCvarPointer("s_a3d"))
		m_pA3DCheckButton->SetVisible(false);
}

COptionsSubAudio::~COptionsSubAudio(void)
{
}

void COptionsSubAudio::OnPageShow(void)
{
}

void COptionsSubAudio::OnResetData(void)
{
	m_pEAXCheckButton->Reset();
	m_pA3DCheckButton->Reset();
	m_pSFXSlider->Reset();
	m_pMusicSlider->Reset();
	m_pSoundQualityCombo->Reset();
}

void COptionsSubAudio::OnApplyChanges(void)
{
	m_pEAXCheckButton->ApplyChanges();
	m_pA3DCheckButton->ApplyChanges();
	m_pSFXSlider->ApplyChanges();
	m_pMusicSlider->ApplyChanges();
	m_pSoundQualityCombo->ApplyChanges();
}

void COptionsSubAudio::OnControlModified(void)
{
	PostActionSignal(new KeyValues("ApplyButtonEnable"));
}

void COptionsSubAudio::OnCommand(const char *command)
{
	if(!stricmp(command, "ShowThirdPartyAudioCredits"))
		OpenThirdPartySoundCreditsDialog();

	BaseClass::OnCommand(command);
}

class COptionsSubAudioThirdPartyCreditsDlg : public vgui::Frame
{
	DECLARE_CLASS_SIMPLE(COptionsSubAudioThirdPartyCreditsDlg, vgui::Frame);

public:
	COptionsSubAudioThirdPartyCreditsDlg(vgui::VPANEL hParent)
	    : BaseClass(NULL, NULL)
	{
		SetTitle("#GameUI_ThirdPartyAudio_Title", true);
		SetSize(500, 200);
		LoadControlSettingsFromScheme("OptionsSubAudioThirdPartyDlg.res");
		MoveToCenterOfScreen();
		SetSizeable(false);
		SetDeleteSelfOnClose(true);
	}

	virtual void Activate(void)
	{
		BaseClass::Activate();

		input()->SetAppModalSurface(GetVPanel());
	}

	void OnKeyCodeTyped(KeyCode code)
	{
		if(code == KEY_ESCAPE)
		{
			SetAlpha(0);
			Close();
		}
		else
			BaseClass::OnKeyCodeTyped(code);
	}
};

void COptionsSubAudio::OpenThirdPartySoundCreditsDialog(void)
{
	if(!m_OptionsSubAudioThirdPartyCreditsDlg.Get())
		m_OptionsSubAudioThirdPartyCreditsDlg = new COptionsSubAudioThirdPartyCreditsDlg(GetVParent());

	m_OptionsSubAudioThirdPartyCreditsDlg->Activate();
}