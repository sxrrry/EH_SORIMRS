class SRGP_InertiaSway_AM : ScriptedWeaponAimModifier
{
	[Attribute("0.1", uiwidget: UIWidgets.Slider, desc: "ALL inertia * this", category: "Settings", params: "0 10")]
	float OVERALL_INERTIA;
	
	[Attribute("1", uiwidget: UIWidgets.Slider, desc: "Yaw (horziontal) inertia * this", category: "Settings", params: "0 10")]
	float INERTIA_HORIZONTAL;
	
	[Attribute("1", uiwidget: UIWidgets.Slider, desc: "Pitch (vertical) inertia * this", category: "Settings", params: "0 10")]
	float INERTIA_VERTICAL;
	
	[Attribute("1", uiwidget: UIWidgets.Slider, desc: "Roll inertia * this", category: "Settings", params: "0 10")]
	float INERTIA_ROLL;
	
	[Attribute("1", uiwidget: UIWidgets.Slider, desc: "Weapon move * this (BETTER TO MATCH WITH INERTIA_HORIZONTAL, BECAUSE THIS CONVERTED FROM IT ((m_vCurrentWeaponAngles[0] - m_vCurrentCameraAngles[0]) * INERTIA_MOVE_HORIZONTAL))", category: "Settings", params: "0 10")]
	float INERTIA_MOVE_HORIZONTAL;
	
	[Attribute("1", uiwidget: UIWidgets.Slider, desc: "Weapon move * this (BETTER TO MATCH WITH INERTIA_VERTICA, BECAUSE THIS CONVERTED FROM IT ((m_vCurrentWeaponAngles[1] - m_vCurrentCameraAngles[1]) * INERTIA_MOVE_VERTICAL))", category: "Settings", params: "0 10")]
	float INERTIA_MOVE_VERTICAL;
	
	[Attribute("0.002", uiwidget: UIWidgets.Slider, desc: "CONVERTED move * this (THIS *-1 to create proper move)", category: "Settings", params: "-1 1")]
	float OVERALL_MOVE;
	
	[Attribute("0.9", uiwidget: UIWidgets.Slider, desc: "Spring effect", category: "Settings", params: "0 1")]
	float INERTIA_SPRING_HORIZONTAL;
	
	[Attribute("0.9", uiwidget: UIWidgets.Slider, desc: "Spring effect", category: "Settings", params: "0 1")]
	float INERTIA_SPRING_VERTICAL;
	
	[Attribute("0.9", uiwidget: UIWidgets.Slider, desc: "Spring damping power", category: "Settings", params: "0 1")]
	float INERTIA_DAMPING_HORIZONTAL;
	
	[Attribute("0.9", uiwidget: UIWidgets.Slider, desc: "Spring damping power", category: "Settings", params: "0 1")]
	float INERTIA_DAMPING_VERTICAL;
	
	[Attribute("15", uiwidget: UIWidgets.Slider, desc: "Speed of all calculations", category: "Settings", params: "1 50")]
	float INERTIA_SPEED;
	
	
	IEntity m_weaponEnt;
	PlayerController m_playerController;
	PlayerCamera m_playerCamera;
	IEntity m_weaponOwner;
	
	vector m_vCurrentCameraAngles;
	vector m_vCurrentWeaponAngles;
	vector m_vPreviousWeaponAngles;
	
	float SPRING_VELOCITY_VERTICAL = 1;
	float SPRING_VELOCITY_HORIZONTAL = 1;
	float prevYawAngle;
	
	override protected void OnInit(IEntity weaponEnt)
	{
		m_weaponEnt = weaponEnt;
	}
	
	override protected void OnActivated(IEntity weaponOwner)
	{
		m_weaponOwner = weaponOwner;
		m_playerController = GetGame().GetPlayerController();
		if (!m_playerController)
			return;
		m_playerCamera = m_playerController.GetPlayerCamera();
		
		if (m_playerCamera)
		{
			m_vCurrentCameraAngles = m_playerCamera.GetLocalYawPitchRoll();
			m_vCurrentWeaponAngles = m_vCurrentCameraAngles;
			m_vPreviousWeaponAngles = m_vCurrentCameraAngles;
		}
	}
	
	override void OnCalculate(IEntity owner, WeaponAimModifierContext context, float timeSlice, out vector translation, out vector rotation, out vector turnOffset)
	{	
		if (!m_playerController)
		{
			m_playerController = GetGame().GetPlayerController();
			return;
		}
		if (!m_playerCamera)
		{
			m_playerCamera = m_playerController.GetPlayerCamera();
			rotation = vector.Zero;
			return;
		}
		
		m_vCurrentCameraAngles = m_playerCamera.GetYawPitchRoll();
		
		// catch and handle harsh 180 -> -180 transition (ya ebal)
		float yawDiff = m_vCurrentCameraAngles[0] - prevYawAngle;
		
		if (yawDiff > 120)
		{
		    // right
		    m_vCurrentWeaponAngles[0] = m_vCurrentWeaponAngles[0] + 360;
		}
		else if (yawDiff < -120)
		{
		    // left
		    m_vCurrentWeaponAngles[0] = m_vCurrentWeaponAngles[0] - 360;
		}
		
		prevYawAngle = m_vCurrentCameraAngles[0];
		
		m_vCurrentWeaponAngles[0] = Math.SmoothSpring(m_vCurrentWeaponAngles[0], m_vCurrentCameraAngles[0], SPRING_VELOCITY_HORIZONTAL, INERTIA_SPRING_HORIZONTAL, INERTIA_DAMPING_HORIZONTAL, timeSlice * INERTIA_SPEED);
		m_vCurrentWeaponAngles[1] = Math.SmoothSpring(m_vCurrentWeaponAngles[1], m_vCurrentCameraAngles[1], SPRING_VELOCITY_VERTICAL, INERTIA_SPRING_VERTICAL, INERTIA_DAMPING_VERTICAL, timeSlice * INERTIA_SPEED);
		
		rotation[0] = Math.Min(Math.Max((m_vCurrentWeaponAngles[0] - m_vCurrentCameraAngles[0]) * INERTIA_HORIZONTAL * OVERALL_INERTIA, -5), 5);
		rotation[1] = Math.Min(Math.Max((m_vCurrentWeaponAngles[1] - m_vCurrentCameraAngles[1]) * INERTIA_VERTICAL * OVERALL_INERTIA, -5), 5);
		rotation[2] = Math.Min(Math.Max((m_vCurrentWeaponAngles[0] - m_vCurrentCameraAngles[0]) * INERTIA_ROLL * OVERALL_INERTIA, -5), 5);
		
		translation[0] = Math.Min(Math.Max((m_vCurrentWeaponAngles[0] - m_vCurrentCameraAngles[0]) * INERTIA_MOVE_HORIZONTAL * OVERALL_INERTIA, -5), 5) * OVERALL_MOVE *-1;
		translation[1] = Math.Min(Math.Max((m_vCurrentWeaponAngles[1] - m_vCurrentCameraAngles[1]) * INERTIA_MOVE_VERTICAL * OVERALL_INERTIA, -5), 5) * OVERALL_MOVE *-1;
		
	}
}