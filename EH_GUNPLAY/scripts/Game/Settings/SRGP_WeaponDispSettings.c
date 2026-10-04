class SRGP_WeaponDisplacementSettings : ModuleGameSettings
{
    [Attribute("0",     uiwidget: UIWidgets.Slider, params: "-180 180 0.1",   desc: "Rotation X (pitch)")]
    float ROTATION_X;

    [Attribute("0",     uiwidget: UIWidgets.Slider, params: "-180 180 0.1",   desc: "Rotation Y (yaw)")]
    float ROTATION_Y;

    [Attribute("-12",   uiwidget: UIWidgets.Slider, params: "-180 180 0.1",   desc: "Rotation Z (roll)")]
    float ROTATION_Z;

    [Attribute("-0.01", uiwidget: UIWidgets.Slider, params: "-0.5 0.5 0.01", desc: "Offset X (right/left)")]
    float OFFSET_X;

    [Attribute("0",     uiwidget: UIWidgets.Slider, params: "-0.5 0.5 0.01",  desc: "Offset Y (up/down)")]
    float OFFSET_Y;

    [Attribute("0",     uiwidget: UIWidgets.Slider, params: "-0.5 0.5 0.01",  desc: "Offset Z (forward/back)")]
    float OFFSET_Z;
}