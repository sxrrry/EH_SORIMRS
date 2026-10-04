class SRGP_WeaponDisplacementSettings : ModuleGameSettings
{
    // Поворот (градусы)
    [Attribute("-0", uiwidget: UIWidgets.Slider, desc: "Rotation X (pitch)", category: "Weapon Displacement", params: "-180 180 0.1")]
    float ROTATION_X;
    
    [Attribute("0", uiwidget: UIWidgets.Slider, desc: "Rotation Y (yaw)", category: "Weapon Displacement", params: "-180 180 0.1")]
    float ROTATION_Y;
    
    [Attribute("-12", uiwidget: UIWidgets.Slider, desc: "Rotation Z (roll)", category: "Weapon Displacement", params: "-180 180 0.1")]
    float ROTATION_Z;
    
    // Смещение (метры)
    [Attribute("-0.01", uiwidget: UIWidgets.Slider, desc: "Offset X (right/left)", category: "Weapon Displacement", params: "-0.5 0.5 0.001")]
    float OFFSET_X;
    
    [Attribute("0", uiwidget: UIWidgets.Slider, desc: "Offset Y (up/down)", category: "Weapon Displacement", params: "-0.5 0.5 0.001")]
    float OFFSET_Y;
    
    [Attribute("0", uiwidget: UIWidgets.Slider, desc: "Offset Z (forward/back)", category: "Weapon Displacement", params: "-0.5 0.5 0.001")]
    float OFFSET_Z;
}