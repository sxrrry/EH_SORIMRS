modded class SCR_TabViewComponent
{
    override protected void CreateTabContent(SCR_TabViewContent content, int index)
    {
        if (content.m_sTabIdentifier == "Settings_SRGP_WeaponDisplacement")
            content.m_ElementLayout = "{ED33315E6A6F2F26}UI/layouts/Menus/SettingsMenu/SRGP_GameplaySettings.layout";

        super.CreateTabContent(content, index);
    }
}

modded class SCR_SettingsSuperMenu
{
    override void OnMenuOpen()
    {
        super.OnMenuOpen();

        if (!m_SuperMenuComponent || !m_SuperMenuComponent.GetTabView())
            return;

        SCR_TabViewComponent tabView = m_SuperMenuComponent.GetTabView();

        tabView.AddTab(
            "{ED33315E6A6F2F26}UI/layouts/Menus/SettingsMenu/SRGP_GameplaySettings.layout",
            "Weapon Displacement",
            true,
            identifier: "Settings_SRGP_WeaponDisplacement"
        );
    }
}