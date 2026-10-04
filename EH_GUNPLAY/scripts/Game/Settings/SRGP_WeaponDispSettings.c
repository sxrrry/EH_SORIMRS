class SRGP_WeaponDisplacementSettings : ModuleGameSettings
{
    [Attribute("0", uiwidget: UIWidgets.CheckBox, desc: "Enable separate settings for handguns")]
    bool ENABLE_PISTOL_SETTINGS;

    [Attribute("0", uiwidget: UIWidgets.Slider, params: "-15 15 0.1", desc: "Rifle: Rotation X (pitch)")]
    float ROTATION_X;

    [Attribute("0", uiwidget: UIWidgets.Slider, params: "-15 15 0.1", desc: "Rifle: Rotation Y (yaw)")]
    float ROTATION_Y;

    [Attribute("0", uiwidget: UIWidgets.Slider, params: "-45 45 0.1", desc: "Rifle: Rotation Z (roll)")]
    float ROTATION_Z;

    [Attribute("0", uiwidget: UIWidgets.Slider, params: "-0.1 0.1 0.005", desc: "Rifle: Offset X (right/left)")]
    float OFFSET_X;

    [Attribute("0", uiwidget: UIWidgets.Slider, params: "-0.1 0.1 0.005", desc: "Rifle: Offset Y (up/down)")]
    float OFFSET_Y;

    [Attribute("0", uiwidget: UIWidgets.Slider, params: "-0.1 0.1 0.005", desc: "Rifle: Offset Z (forward/back)")]
    float OFFSET_Z;

    [Attribute("0", uiwidget: UIWidgets.Slider, params: "-15 15 0.1", desc: "Pistol: Rotation X (pitch)")]
    float PISTOL_ROTATION_X;

    [Attribute("0", uiwidget: UIWidgets.Slider, params: "-15 15 0.1", desc: "Pistol: Rotation Y (yaw)")]
    float PISTOL_ROTATION_Y;

    [Attribute("0", uiwidget: UIWidgets.Slider, params: "-45 45 0.1", desc: "Pistol: Rotation Z (roll)")]
    float PISTOL_ROTATION_Z;

    [Attribute("0", uiwidget: UIWidgets.Slider, params: "-0.1 0.1 0.005", desc: "Pistol: Offset X (right/left)")]
    float PISTOL_OFFSET_X;

    [Attribute("0", uiwidget: UIWidgets.Slider, params: "-0.1 0.1 0.005", desc: "Pistol: Offset Y (up/down)")]
    float PISTOL_OFFSET_Y;

    [Attribute("0", uiwidget: UIWidgets.Slider, params: "-0.2 0.2 0.005", desc: "Pistol: Offset Z (forward/back)")]
    float PISTOL_OFFSET_Z;

    [Attribute("0", uiwidget: UIWidgets.Slider, params: "0 2 1", desc: "Preset slot index (0-2)")]
    int PRESET_SLOT;
}