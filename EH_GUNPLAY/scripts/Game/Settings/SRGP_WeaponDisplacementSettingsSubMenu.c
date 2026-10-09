class SRGP_WeaponDisplacementSettingsSubMenu : SCR_SettingsSubMenuBase
{
    ref RichTextWidget m_wDescText;
    ref map<string, string> m_mDescriptions = new map<string, string>();
    ref array<string> m_aPistolWidgets = new array<string>();

    protected int m_iLastPresetSlot = 1;
    protected bool m_bLastPistolEnabled = false;
    protected bool m_bApplyingPreset = false;
    protected bool m_bNormalizing = false;
    protected bool m_bNormalizeTimerRunning = false;

    override void OnTabHide()
    {
        StopNormalizeLoop();

        super.OnTabHide();

        SCR_PlayerController pc = SCR_PlayerController.Cast(GetGame().GetPlayerController());
        if (pc)
            pc.SetGameUserSettings();

        GetGame().SaveUserSettings();
    }

    override void OnTabCreate(Widget menuRoot, ResourceName buttonsLayout, int index)
    {
        super.OnTabCreate(menuRoot, buttonsLayout, index);

        if (!m_wRoot)
            return;

        m_wDescText = RichTextWidget.Cast(m_wRoot.FindAnyWidget("m_wText_Desc"));
        if (m_wDescText)
            m_wDescText.SetText("");

        m_aSettingsBindings.Clear();

        SRGP_WeaponDisplacementPersistence persistence = new SRGP_WeaponDisplacementPersistence();
        persistence.LoadFromFileOrDefaults();
        persistence.ApplyToGameSettings();

        BaseContainer settings = GetGame().GetGameUserSettings().GetModule("SRGP_WeaponDisplacementSettings");
        if (settings)
        {
            int slotIdx;
            settings.Get("PRESET_SLOT", slotIdx);
            if (slotIdx < 0 || slotIdx > 2)
                slotIdx = 0;
            m_iLastPresetSlot = slotIdx + 1;

            bool enablePistol;
            settings.Get("ENABLE_PISTOL_SETTINGS", enablePistol);
            m_bLastPistolEnabled = enablePistol;
        }

        RegisterPresetCarousel();

        RegisterSlider("wSetting_SRGP_ROTATION_X", "ROTATION_X",
            "Rifle: Rotation X (pitch)",
            "Rotates the rifle around the X axis (up / down), in degrees.",
            -15, 15, 0.1);

        RegisterSlider("wSetting_SRGP_ROTATION_Y", "ROTATION_Y",
            "Rifle: Rotation Y (yaw)",
            "Rotates the rifle around the Y axis (left / right), in degrees.",
            -15, 15, 0.1);

        RegisterSlider("wSetting_SRGP_ROTATION_Z", "ROTATION_Z",
            "Rifle: Rotation Z (roll)",
            "Rotates the rifle around the Z axis (roll), in degrees.",
            -45, 45, 0.1);

        RegisterSlider("wSetting_SRGP_OFFSET_X", "OFFSET_X",
            "Rifle: Offset X (right / left)",
            "Shifts the rifle along the X axis (right / left), in meters.",
            -0.1, 0.1, 0.005);

        RegisterSlider("wSetting_SRGP_OFFSET_Y", "OFFSET_Y",
            "Rifle: Offset Y (up / down)",
            "Shifts the rifle along the Y axis (up / down), in meters.",
            -0.1, 0.1, 0.005);

        RegisterSlider("wSetting_SRGP_OFFSET_Z", "OFFSET_Z",
            "Rifle: Offset Z (forward / back)",
            "Shifts the rifle along the Z axis (forward / back), in meters.",
            -0.1, 0.1, 0.005);

        RegisterCheckbox("wSetting_SRGP_ENABLE_PISTOL_SETTINGS", "ENABLE_PISTOL_SETTINGS",
            "Enable separate handgun settings",
            "When enabled, handguns use their own displacement values below. When disabled, handguns use the rifle values above.");

        RegisterSlider("wSetting_SRGP_PISTOL_ROTATION_X", "PISTOL_ROTATION_X",
            "Pistol: Rotation X (pitch)",
            "Rotates the pistol around the X axis (up / down), in degrees.",
            -15, 15, 0.1);
        m_aPistolWidgets.Insert("wSetting_SRGP_PISTOL_ROTATION_X");

        RegisterSlider("wSetting_SRGP_PISTOL_ROTATION_Y", "PISTOL_ROTATION_Y",
            "Pistol: Rotation Y (yaw)",
            "Rotates the pistol around the Y axis (left / right), in degrees.",
            -15, 15, 0.1);
        m_aPistolWidgets.Insert("wSetting_SRGP_PISTOL_ROTATION_Y");

        RegisterSlider("wSetting_SRGP_PISTOL_ROTATION_Z", "PISTOL_ROTATION_Z",
            "Pistol: Rotation Z (roll)",
            "Rotates the pistol around the Z axis (roll), in degrees.",
            -45, 45, 0.1);
        m_aPistolWidgets.Insert("wSetting_SRGP_PISTOL_ROTATION_Z");

        RegisterSlider("wSetting_SRGP_PISTOL_OFFSET_X", "PISTOL_OFFSET_X",
            "Pistol: Offset X (right / left)",
            "Shifts the pistol along the X axis (right / left), in meters.",
            -0.1, 0.1, 0.005);
        m_aPistolWidgets.Insert("wSetting_SRGP_PISTOL_OFFSET_X");

        RegisterSlider("wSetting_SRGP_PISTOL_OFFSET_Y", "PISTOL_OFFSET_Y",
            "Pistol: Offset Y (up / down)",
            "Shifts the pistol along the Y axis (up / down), in meters.",
            -0.1, 0.1, 0.005);
        m_aPistolWidgets.Insert("wSetting_SRGP_PISTOL_OFFSET_Y");

        RegisterSlider("wSetting_SRGP_PISTOL_OFFSET_Z", "PISTOL_OFFSET_Z",
            "Pistol: Offset Z (forward / back)",
            "Shifts the pistol along the Z axis (forward / back), in meters.",
            -0.2, 0.2, 0.005);
        m_aPistolWidgets.Insert("wSetting_SRGP_PISTOL_OFFSET_Z");

        LoadSettings();
        ApplyPistolVisibility();
        SyncToComponent();
        StartNormalizeLoop();

        Widget resetBtn = m_wRoot.FindAnyWidget("wButton_ResetDefaults");
        if (resetBtn)
        {
            SCR_ModularButtonComponent resetComp = SCR_ModularButtonComponent.FindComponent(resetBtn);
            if (resetComp)
                resetComp.m_OnClicked.Insert(OnResetDefaults);
        }
    }

    override protected void OnMenuItemChanged(SCR_SettingsBindingBase binding)
    {
        super.OnMenuItemChanged(binding);

        GetGame().GetCallqueue().CallLater(OnAnySettingChanged, 0, false);
        GetGame().GetCallqueue().CallLater(SyncToComponent, 0, false);
    }

    protected void StartNormalizeLoop()
    {
        if (m_bNormalizeTimerRunning)
            return;

        m_bNormalizeTimerRunning = true;
        GetGame().GetCallqueue().CallLater(TickNormalizeLoop, 30, false);
    }

    protected void StopNormalizeLoop()
    {
        m_bNormalizeTimerRunning = false;
        GetGame().GetCallqueue().Remove(TickNormalizeLoop);
    }

    protected void TickNormalizeLoop()
    {
        if (!m_bNormalizeTimerRunning)
            return;

        NormalizeAllSliders();

        GetGame().GetCallqueue().CallLater(TickNormalizeLoop, 30, false);
    }

    protected void NormalizeAllSliders()
    {
        if (m_bNormalizing)
            return;

        m_bNormalizing = true;

        BaseContainer s = GetGame().GetGameUserSettings().GetModule("SRGP_WeaponDisplacementSettings");
        if (s)
        {
            RoundAndSet(s, "ROTATION_X", 0.1, "wSetting_SRGP_ROTATION_X");
            RoundAndSet(s, "ROTATION_Y", 0.1, "wSetting_SRGP_ROTATION_Y");
            RoundAndSet(s, "ROTATION_Z", 0.1, "wSetting_SRGP_ROTATION_Z");

            RoundAndSet(s, "OFFSET_X", 0.005, "wSetting_SRGP_OFFSET_X");
            RoundAndSet(s, "OFFSET_Y", 0.005, "wSetting_SRGP_OFFSET_Y");
            RoundAndSet(s, "OFFSET_Z", 0.005, "wSetting_SRGP_OFFSET_Z");

            RoundAndSet(s, "PISTOL_ROTATION_X", 0.1, "wSetting_SRGP_PISTOL_ROTATION_X");
            RoundAndSet(s, "PISTOL_ROTATION_Y", 0.1, "wSetting_SRGP_PISTOL_ROTATION_Y");
            RoundAndSet(s, "PISTOL_ROTATION_Z", 0.1, "wSetting_SRGP_PISTOL_ROTATION_Z");

            RoundAndSet(s, "PISTOL_OFFSET_X", 0.005, "wSetting_SRGP_PISTOL_OFFSET_X");
            RoundAndSet(s, "PISTOL_OFFSET_Y", 0.005, "wSetting_SRGP_PISTOL_OFFSET_Y");
            RoundAndSet(s, "PISTOL_OFFSET_Z", 0.005, "wSetting_SRGP_PISTOL_OFFSET_Z");
        }

        m_bNormalizing = false;
    }

        protected void RoundAndSet(BaseContainer s, string varName, float step, string widgetName)
    {
        float v;
        if (!s.Get(varName, v))
            return;

        float rounded = SRGP_WeaponDisplacementPersistence.RoundToStep(v, step);

        if (v != rounded)
            s.Set(varName, rounded);
    }

    protected void OnAnySettingChanged()
    {
        if (m_bApplyingPreset)
            return;

        ApplyPistolVisibility();

        BaseContainer settings = GetGame().GetGameUserSettings().GetModule("SRGP_WeaponDisplacementSettings");
        if (!settings)
            return;

        bool enablePistol = false;
        settings.Get("ENABLE_PISTOL_SETTINGS", enablePistol);
        m_bLastPistolEnabled = enablePistol;

        int slotIdx = 0;
        settings.Get("PRESET_SLOT", slotIdx);
        if (slotIdx < 0) slotIdx = 0;
        if (slotIdx > 2) slotIdx = 2;

        int slot = slotIdx + 1;

        if (slot != m_iLastPresetSlot)
        {
            GetGame().GetCallqueue().CallLater(OnPresetSlotChanged, 0, false, slot);
        }
        else
        {
            GetGame().GetCallqueue().CallLater(SaveToCurrentSlot, 50, false);
        }
    }

    protected void ApplyPistolVisibility()
    {
        BaseContainer settings = GetGame().GetGameUserSettings().GetModule("SRGP_WeaponDisplacementSettings");
        if (!settings)
            return;

        bool enablePistol = false;
        settings.Get("ENABLE_PISTOL_SETTINGS", enablePistol);

        foreach (string widgetName : m_aPistolWidgets)
        {
            Widget w = m_wRoot.FindAnyWidget(widgetName);
            if (w)
                w.SetVisible(enablePistol);
        }
    }

    protected void OnSliderChangedFinal(SCR_SliderComponent comp, float val)
    {
        if (m_bApplyingPreset)
            return;

        if (!comp || !comp.GetRootWidget())
            return;

        GetGame().GetCallqueue().CallLater(SaveToCurrentSlot, 50, false);
        GetGame().GetCallqueue().CallLater(RefreshAllWidgets, 50, false);
    }

    protected void OnPresetSlotChanged(int newSlot)
    {
        if (newSlot < 1 || newSlot > 3)
            return;

        if (m_iLastPresetSlot != newSlot)
            SaveToSlot(m_iLastPresetSlot);

        m_bApplyingPreset = true;

        SRGP_WeaponDisplacementPersistence persistence = new SRGP_WeaponDisplacementPersistence();

        if (!persistence.LoadFromSlot(newSlot))
        {
            persistence.ENABLE_PISTOL_SETTINGS = false;
            persistence.ROTATION_X = 0;
            persistence.ROTATION_Y = 0;
            persistence.ROTATION_Z = 0;
            persistence.OFFSET_X = 0;
            persistence.OFFSET_Y = 0;
            persistence.OFFSET_Z = 0;
            persistence.PISTOL_ROTATION_X = 0;
            persistence.PISTOL_ROTATION_Y = 0;
            persistence.PISTOL_ROTATION_Z = 0;
            persistence.PISTOL_OFFSET_X = 0;
            persistence.PISTOL_OFFSET_Y = 0;
            persistence.PISTOL_OFFSET_Z = 0;
            persistence.SaveToSlot(newSlot);
        }

        persistence.ApplyToGameSettings();

        RefreshAllWidgets();
        ApplyPistolVisibility();

        m_iLastPresetSlot = newSlot;

        SyncToComponent();
        SaveToJson();

        m_bApplyingPreset = false;
    }

    protected void RefreshAllWidgets()
    {
        BaseContainer s = GetGame().GetGameUserSettings().GetModule("SRGP_WeaponDisplacementSettings");
        if (!s)
            return;

        SetCheckboxValue("wSetting_SRGP_ENABLE_PISTOL_SETTINGS", "ENABLE_PISTOL_SETTINGS", s);

        SetSliderValue("wSetting_SRGP_ROTATION_X", "ROTATION_X", s);
        SetSliderValue("wSetting_SRGP_ROTATION_Y", "ROTATION_Y", s);
        SetSliderValue("wSetting_SRGP_ROTATION_Z", "ROTATION_Z", s);
        SetSliderValue("wSetting_SRGP_OFFSET_X", "OFFSET_X", s);
        SetSliderValue("wSetting_SRGP_OFFSET_Y", "OFFSET_Y", s);
        SetSliderValue("wSetting_SRGP_OFFSET_Z", "OFFSET_Z", s);

        SetSliderValue("wSetting_SRGP_PISTOL_ROTATION_X", "PISTOL_ROTATION_X", s);
        SetSliderValue("wSetting_SRGP_PISTOL_ROTATION_Y", "PISTOL_ROTATION_Y", s);
        SetSliderValue("wSetting_SRGP_PISTOL_ROTATION_Z", "PISTOL_ROTATION_Z", s);
        SetSliderValue("wSetting_SRGP_PISTOL_OFFSET_X", "PISTOL_OFFSET_X", s);
        SetSliderValue("wSetting_SRGP_PISTOL_OFFSET_Y", "PISTOL_OFFSET_Y", s);
        SetSliderValue("wSetting_SRGP_PISTOL_OFFSET_Z", "PISTOL_OFFSET_Z", s);
    }

    protected void SetSliderValue(string widgetName, string varName, BaseContainer s)
    {
        Widget w = m_wRoot.FindAnyWidget(widgetName);
        if (!w)
            return;

        SCR_SliderComponent sl = SCR_SliderComponent.Cast(w.FindHandler(SCR_SliderComponent));
        if (!sl)
            return;

        float v;
        if (!s.Get(varName, v))
            return;

        sl.SetValue(v);
    }

    protected void SetCheckboxValue(string widgetName, string varName, BaseContainer s)
    {
        Widget w = m_wRoot.FindAnyWidget(widgetName);
        if (!w)
            return;

        SCR_SpinBoxComponent spinBox = SCR_SpinBoxComponent.Cast(w.FindHandler(SCR_SpinBoxComponent));
        if (!spinBox)
            return;

        bool v;
        if (!s.Get(varName, v))
            return;

        int index = 0;
        if (v)
            index = 1;

        spinBox.SetCurrentItem(index);
    }

    protected void SaveToCurrentSlot()
    {
        SaveToSlot(m_iLastPresetSlot);
        SaveToJson();
    }

    protected void SaveToSlot(int slot)
    {
        if (slot < 1 || slot > 3)
            return;

        SRGP_WeaponDisplacementPersistence persistence = new SRGP_WeaponDisplacementPersistence();
        persistence.LoadFromGameSettings();
        persistence.SaveToSlot(slot);
    }

    protected void SaveToJson()
    {
        SRGP_WeaponDisplacementPersistence persistence = new SRGP_WeaponDisplacementPersistence();
        persistence.LoadFromGameSettings();
        persistence.SaveToFile();
    }

    protected void RegisterPresetCarousel()
    {
        if (!m_aSettingsBindings || !m_wRoot)
            return;

        Widget content = m_wRoot.FindAnyWidget("Content");
        if (!content)
            return;

        string widgetName = "wSetting_SRGP_PRESET_SLOT";

        Widget settingWidget = m_wRoot.FindAnyWidget(widgetName);
        if (!settingWidget)
        {
            ResourceName boxLayout = "{C9DF0E6590F6C388}UI/layouts/WidgetLibrary/SpinBox/WLib_SpinBox.layout";

            settingWidget = GetGame().GetWorkspace().CreateWidgets(boxLayout, content);
            settingWidget.SetName(widgetName);

            SCR_SpinBoxComponent spinBox = SCR_SpinBoxComponent.Cast(settingWidget.FindHandler(SCR_SpinBoxComponent));
            if (spinBox)
            {
                spinBox.SetLabel("Preset Slot");
                spinBox.AddItem("Preset 1");
                spinBox.AddItem("Preset 2");
                spinBox.AddItem("Preset 3");
            }
        }

        SCR_SettingBindingGameplay binding = new SCR_SettingBindingGameplay(
            "SRGP_WeaponDisplacementSettings",
            "PRESET_SLOT",
            widgetName
        );
        m_aSettingsBindings.Insert(binding);

        Widget finalWidget = m_wRoot.FindAnyWidget(widgetName);
        if (finalWidget)
        {
            SCR_ModularButtonComponent btn = SCR_ModularButtonComponent.Cast(finalWidget.FindHandler(SCR_ModularButtonComponent));
            if (btn)
                btn.m_OnFocus.Insert(OnSettingFocused);
        }

        m_mDescriptions.Insert(widgetName, "Selects the active preset slot. Switching slots auto-saves the current configuration and loads the selected one.");
    }

    protected void RegisterSlider(
        string widgetName,
        string settingVar,
        string label,
        string description,
        float minValue,
        float maxValue,
        float step
    )
    {
        if (!m_aSettingsBindings || !m_wRoot)
            return;

        Widget content = m_wRoot.FindAnyWidget("Content");
        if (!content)
            return;

        Widget settingWidget = m_wRoot.FindAnyWidget(widgetName);
        if (!settingWidget)
        {
            ResourceName sliderLayout = "{4A41296C0E9A889F}UI/layouts/WidgetLibrary/WLib_Slider.layout";

            settingWidget = GetGame().GetWorkspace().CreateWidgets(sliderLayout, content);
            settingWidget.SetName(widgetName);

            SCR_SliderComponent slider = SCR_SliderComponent.Cast(settingWidget.FindHandler(SCR_SliderComponent));
            if (slider)
            {
                slider.SetLabel(label);
                slider.SetSliderSettings(minValue, maxValue, step);
                slider.SetFormatText("%1");
            }
        }

        SCR_SettingBindingGameplay binding = new SCR_SettingBindingGameplay(
            "SRGP_WeaponDisplacementSettings",
            settingVar,
            widgetName
        );
        m_aSettingsBindings.Insert(binding);

        Widget finalWidget = m_wRoot.FindAnyWidget(widgetName);
        if (finalWidget)
        {
            SCR_ModularButtonComponent btn = SCR_ModularButtonComponent.Cast(finalWidget.FindHandler(SCR_ModularButtonComponent));
            if (btn)
                btn.m_OnFocus.Insert(OnSettingFocused);

            SCR_SliderComponent sliderComp = SCR_SliderComponent.Cast(finalWidget.FindHandler(SCR_SliderComponent));
            if (sliderComp)
                sliderComp.GetOnChangedFinal().Insert(OnSliderChangedFinal);
        }

        m_mDescriptions.Insert(widgetName, description);
    }

    protected void RegisterCheckbox(
        string widgetName,
        string settingVar,
        string label,
        string description
    )
    {
        if (!m_aSettingsBindings || !m_wRoot)
            return;

        Widget content = m_wRoot.FindAnyWidget("Content");
        if (!content)
            return;

        Widget settingWidget = m_wRoot.FindAnyWidget(widgetName);
        if (!settingWidget)
        {
            ResourceName boxLayout = "{C9DF0E6590F6C388}UI/layouts/WidgetLibrary/SpinBox/WLib_SpinBox.layout";

            settingWidget = GetGame().GetWorkspace().CreateWidgets(boxLayout, content);
            settingWidget.SetName(widgetName);

            SCR_SpinBoxComponent spinBox = SCR_SpinBoxComponent.Cast(settingWidget.FindHandler(SCR_SpinBoxComponent));
            if (spinBox)
            {
                spinBox.SetLabel(label);
                spinBox.SetCycleMode(true);
                spinBox.AddItem("No");
                spinBox.AddItem("Yes");
            }
        }

        SCR_SettingBindingGameplay binding = new SCR_SettingBindingGameplay(
            "SRGP_WeaponDisplacementSettings",
            settingVar,
            widgetName
        );
        m_aSettingsBindings.Insert(binding);

        Widget finalWidget = m_wRoot.FindAnyWidget(widgetName);
        if (finalWidget)
        {
            SCR_ModularButtonComponent btn = SCR_ModularButtonComponent.Cast(finalWidget.FindHandler(SCR_ModularButtonComponent));
            if (btn)
                btn.m_OnFocus.Insert(OnSettingFocused);
        }

        m_mDescriptions.Insert(widgetName, description);
    }

    protected void OnSettingFocused(SCR_ModularButtonComponent button)
    {
        if (!button || !button.GetRootWidget() || !m_wDescText)
            return;

        string widgetName = button.GetRootWidget().GetName();
        string description;
        if (m_mDescriptions.Find(widgetName, description))
            m_wDescText.SetText(description);
    }

    protected void OnResetDefaults()
    {
        BaseContainer settings = GetGame().GetGameUserSettings().GetModule("SRGP_WeaponDisplacementSettings");
        if (!settings)
            return;

        settings.Set("ROTATION_X", 0);
        settings.Set("ROTATION_Y", 0);
        settings.Set("ROTATION_Z", 0);
        settings.Set("OFFSET_X", 0);
        settings.Set("OFFSET_Y", 0);
        settings.Set("OFFSET_Z", 0);

        settings.Set("PISTOL_ROTATION_X", 0);
        settings.Set("PISTOL_ROTATION_Y", 0);
        settings.Set("PISTOL_ROTATION_Z", 0);
        settings.Set("PISTOL_OFFSET_X", 0);
        settings.Set("PISTOL_OFFSET_Y", 0);
        settings.Set("PISTOL_OFFSET_Z", 0);

        RefreshAllWidgets();
        ApplyPistolVisibility();
        SyncToComponent();
        SaveToCurrentSlot();
        GetGame().GetCallqueue().CallLater(PersistSettings, 100, false);
    }

    protected void SyncToComponent()
    {
        SCR_PlayerController pc = SCR_PlayerController.Cast(GetGame().GetPlayerController());
        if (!pc)
            return;

        IEntity character = pc.GetControlledEntity();
        if (!character)
            return;

        SRGP_WeaponDisplacementComponent comp =
            SRGP_WeaponDisplacementComponent.Cast(character.FindComponent(SRGP_WeaponDisplacementComponent));
        if (!comp)
            return;

        BaseContainer settings = GetGame().GetGameUserSettings().GetModule("SRGP_WeaponDisplacementSettings");
        if (!settings)
            return;

        float rotX, rotY, rotZ, offX, offY, offZ;
        float pRotX, pRotY, pRotZ, pOffX, pOffY, pOffZ;
        bool enablePistol;

        settings.Get("ROTATION_X", rotX);
        settings.Get("ROTATION_Y", rotY);
        settings.Get("ROTATION_Z", rotZ);
        settings.Get("OFFSET_X", offX);
        settings.Get("OFFSET_Y", offY);
        settings.Get("OFFSET_Z", offZ);

        settings.Get("PISTOL_ROTATION_X", pRotX);
        settings.Get("PISTOL_ROTATION_Y", pRotY);
        settings.Get("PISTOL_ROTATION_Z", pRotZ);
        settings.Get("PISTOL_OFFSET_X", pOffX);
        settings.Get("PISTOL_OFFSET_Y", pOffY);
        settings.Get("PISTOL_OFFSET_Z", pOffZ);

        settings.Get("ENABLE_PISTOL_SETTINGS", enablePistol);

        comp.RequestApplyValues(
            rotX, rotY, rotZ, offX, offY, offZ,
            pRotX, pRotY, pRotZ, pOffX, pOffY, pOffZ,
            enablePistol
        );
    }

    protected void PersistSettings()
    {
        SCR_PlayerController pc = SCR_PlayerController.Cast(GetGame().GetPlayerController());
        if (pc)
            pc.SetGameUserSettings();

        GetGame().SaveUserSettings();
    }
}