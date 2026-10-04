class SRGP_WeaponDisplacementSettingsSubMenu : SCR_SettingsSubMenuBase
{
    ref RichTextWidget m_wDescText;
    ref map<string, string> m_mDescriptions = new map<string, string>();

    override void OnTabHide()
    {
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

        RegisterSlider("wSetting_SRGP_ROTATION_X", "ROTATION_X",
            "Rotation X (pitch)",
            "Rotates the weapon around the X axis (up / down), in degrees. (AFFECTS AIM WHEN HIPFIRING!)",
            -15, 15, 0.1);

        RegisterSlider("wSetting_SRGP_ROTATION_Y", "ROTATION_Y",
            "Rotation Y (yaw)",
            "Rotates the weapon around the Y axis (left / right), in degrees. (AFFECTS AIM WHEN HIPFIRING!)",
            -15, 15, 0.1);

        RegisterSlider("wSetting_SRGP_ROTATION_Z", "ROTATION_Z",
            "Rotation Z (roll)",
            "Rotates the weapon around the Z axis (roll), in degrees.",
            -45, 45, 0.1);

        RegisterSlider("wSetting_SRGP_OFFSET_X", "OFFSET_X",
            "Offset X (right / left)",
            "Shifts the weapon along the X axis (right / left), in meters.",
            -0.1, 0.1, 0.005);

        RegisterSlider("wSetting_SRGP_OFFSET_Y", "OFFSET_Y",
            "Offset Y (up / down)",
            "Shifts the weapon along the Y axis (up / down), in meters.",
            -0.1, 0.1, 0.005);

        RegisterSlider("wSetting_SRGP_OFFSET_Z", "OFFSET_Z",
            "Offset Z (forward / back)",
            "Shifts the weapon along the Z axis (forward / back), in meters.",
            -0.1, 0.1, 0.005);

        LoadSettings();
        SyncToComponent();

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

        GetGame().GetCallqueue().CallLater(SyncToComponent, 0, false);
    }

    protected void OnSliderChangedFinal(SCR_SliderComponent comp, float val)
    {
        GetGame().GetCallqueue().CallLater(SaveToJson, 50, false);
    }

    protected void SaveToJson()
    {
        SRGP_WeaponDisplacementPersistence persistence = new SRGP_WeaponDisplacementPersistence();
        persistence.LoadFromGameSettings();
        persistence.SaveToFile();
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

        LoadSettings();
        SyncToComponent();
        SaveToJson();
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
        settings.Get("ROTATION_X", rotX);
        settings.Get("ROTATION_Y", rotY);
        settings.Get("ROTATION_Z", rotZ);
        settings.Get("OFFSET_X", offX);
        settings.Get("OFFSET_Y", offY);
        settings.Get("OFFSET_Z", offZ);

        comp.RequestApplyValues(rotX, rotY, rotZ, offX, offY, offZ);
    }

    protected void PersistSettings()
    {
        SCR_PlayerController pc = SCR_PlayerController.Cast(GetGame().GetPlayerController());
        if (pc)
            pc.SetGameUserSettings();

        GetGame().SaveUserSettings();
    }
}