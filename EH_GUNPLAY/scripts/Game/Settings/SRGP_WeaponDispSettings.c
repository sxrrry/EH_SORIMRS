class SRGP_WeaponDisplacementSettings : ModuleGameSettings
{
    [Attribute("0",     uiwidget: UIWidgets.Slider, params: "-15 15 0.1",   desc: "Rotation X (pitch) (AFFECTS AIM WHEN HIPFIRING!)")]
    float ROTATION_X;

    [Attribute("0",     uiwidget: UIWidgets.Slider, params: "-15 15 0.1",   desc: "Rotation Y (yaw) (AFFECTS AIM WHEN HIPFIRING!)")]
    float ROTATION_Y;

    [Attribute("0",   uiwidget: UIWidgets.Slider, params: "-45 45 0.1",   desc: "Rotation Z (roll)")]
    float ROTATION_Z;

    [Attribute("0", uiwidget: UIWidgets.Slider, params: "-0.1 0.1 0.005", desc: "Offset X (right/left)")]
    float OFFSET_X;

    [Attribute("0",     uiwidget: UIWidgets.Slider, params: "-0.1 0.1 0.005",  desc: "Offset Y (up/down)")]
    float OFFSET_Y;

    [Attribute("0",     uiwidget: UIWidgets.Slider, params: "-0.1 0.1 0.005",  desc: "Offset Z (forward/back)")]
    float OFFSET_Z;
}