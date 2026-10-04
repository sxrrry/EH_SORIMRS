class SRGP_WeaponDisplacement_AM : ScriptedWeaponAimModifier
{
	/*
	[Attribute("0", uiwidget: UIWidgets.Slider, desc: "ALL inertia * this", category: "Settings", params: "-180 180")]
	float ROTATION_X;
	[Attribute("0", uiwidget: UIWidgets.Slider, desc: "ALL inertia * this", category: "Settings", params: "-180 180")]
	float ROTATION_Y;
	[Attribute("-12", uiwidget: UIWidgets.Slider, desc: "ALL inertia * this", category: "Settings", params: "-180 180")]
	float ROTATION_Z;
	
	[Attribute("-0.01", uiwidget: UIWidgets.Slider, desc: "ALL inertia * this", category: "Settings", params: "-0.5 0.5")]
	float OFFSET_X;
	[Attribute("0", uiwidget: UIWidgets.Slider, desc: "ALL inertia * this", category: "Settings", params: "-0.5 0.5")]
	float OFFSET_Y;
	[Attribute("0", uiwidget: UIWidgets.Slider, desc: "ALL inertia * this", category: "Settings", params: "-0.5 0.5")]
	float OFFSET_Z;
	*/
	
	IEntity m_weaponOwner;
	IEntity m_weaponEnt;
	
	float SPRING_VELOCITY = 0;
	ref SRGP_WeaponDisplacementSettings m_Settings;
	
	float m_fCurrentMult;
	float m_fTargetMult;
	
	override protected void OnActivated(IEntity weaponOwner)
	{
		m_weaponOwner = weaponOwner;
		
		// Создаём пустой экземпляр нашего класса настроек
	    m_Settings = new SRGP_WeaponDisplacementSettings();
	
	    // Копируем в него значения из контейнера, который возвращает движок
	    BaseContainerTools.ReadFromInstance(
	        m_Settings,
	        GetGame().GetGameUserSettings().GetModule("SRGP_WeaponDisplacementSettings")
	    );
	}
	
	override void OnCalculate(IEntity owner, WeaponAimModifierContext context, float timeSlice, out vector translation, out vector rotation, out vector turnOffset)
	{
		if (!m_weaponOwner)
			return;
		if (SRGP_Utils.SRGP_IsInADS(m_weaponOwner))
			m_fTargetMult = 0;
		else
			m_fTargetMult = 1;
		
		m_fCurrentMult = Math.SmoothSpring(m_fCurrentMult, m_fTargetMult, SPRING_VELOCITY, 0.1, 0.5, timeSlice * 35);
		
		translation[0] = m_Settings.OFFSET_X * m_fCurrentMult;
        translation[1] = m_Settings.OFFSET_Y * m_fCurrentMult;
        translation[2] = m_Settings.OFFSET_Z * m_fCurrentMult;
        
        rotation[0] = m_Settings.ROTATION_X * m_fCurrentMult;
        rotation[1] = m_Settings.ROTATION_Y * m_fCurrentMult;
        rotation[2] = m_Settings.ROTATION_Z * m_fCurrentMult;
	}
}