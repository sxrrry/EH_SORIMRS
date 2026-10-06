class SRGP_MovementSway_AM : ScriptedWeaponAimModifier
{
	[Attribute("0.6", uiwidget: UIWidgets.Slider, desc: "ALL inertia * this", category: "Settings", params: "0 10")]
	float OVERALL_INERTIA;
	[Attribute("1", uiwidget: UIWidgets.Slider, desc: "ALL inertia * this", category: "Settings", params: "0 10")]
	float INERTIA_ROLL;
	[Attribute("0.5", uiwidget: UIWidgets.Slider, desc: "ALL inertia * this", category: "Settings", params: "0 10")]
	float SWAY_ADS_POWER;
	
	[Attribute("0.4", uiwidget: UIWidgets.Slider, desc: "ALL inertia * this", category: "Settings", params: "0 10")]
	float SWAY_SPRING_VERTICAL;
	[Attribute("0.4", uiwidget: UIWidgets.Slider, desc: "ALL inertia * this", category: "Settings", params: "0 10")]
	float SWAY_SPRING_HORIZONTAL;
	[Attribute("0.4", uiwidget: UIWidgets.Slider, desc: "ALL inertia * this", category: "Settings", params: "0 10")]
	float SWAY_DAMPING_VERTICAL;
	[Attribute("0.4", uiwidget: UIWidgets.Slider, desc: "ALL inertia * this", category: "Settings", params: "0 10")]
	float SWAY_DAMPING_HORIZONTAL;
	[Attribute("15", uiwidget: UIWidgets.Slider, desc: "Speed of all calculations", category: "Settings", params: "1 50")]
	float SWAY_SPEED;
	
	IEntity m_weaponOwner;
	IEntity m_weaponEnt;
	PlayerController m_playerController;
	SCR_CharacterControllerComponent m_characterControllerComponent;
	PlayerCamera m_playerCamera;
	
	float m_fCurrentHorizontalSway;
	float m_fCurrentVerticalSway;
	float m_fTargetHorizontalSway;
	float m_fTargetVerticalSway;
	float m_fCompensation;
	float m_fADSPower;
	float SPRING_VELOCITY_VERTICAL = 0;
	float SPRING_VELOCITY_HORIZONTAL = 0;
	
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
		
		m_characterControllerComponent = SCR_CharacterControllerComponent.Cast(m_weaponOwner.FindComponent(SCR_CharacterControllerComponent));
	}
	
	override void OnCalculate(IEntity owner, WeaponAimModifierContext context, float timeSlice, out vector translation, out vector rotation, out vector turnOffset)
	{
		timeSlice = Math.Min(timeSlice, 0.033);
		translation = vector.Zero;
		rotation = vector.Zero;
		turnOffset = vector.Zero;
		
		if (!m_playerCamera)
		{
			if (m_playerController)
			{
				m_playerCamera = m_playerController.GetPlayerCamera();
				return;
			}
			else
			{
				m_playerController = GetGame().GetPlayerController();
				return;
			}
		}
		if (!m_characterControllerComponent)
		{
			m_characterControllerComponent = SCR_CharacterControllerComponent.Cast(m_weaponOwner.FindComponent(SCR_CharacterControllerComponent));
			return;
		}
		
		vector currentVelocityWorld = m_characterControllerComponent.GetVelocity();
		vector camMatrix[4];
		
		m_playerCamera.GetWorldCameraTransform(camMatrix);
		float localVelRight   = vector.Dot(currentVelocityWorld, camMatrix[0]);
	    float localVelForward = vector.Dot(currentVelocityWorld, camMatrix[2]);
		
		m_fTargetHorizontalSway = localVelRight * 0.01;
	    m_fTargetVerticalSway   = localVelForward * 0.01;
		
		if (SRGP_Utils.SRGP_IsInADS(m_weaponOwner))
		{
			m_fTargetHorizontalSway *= 0.3;
			m_fTargetVerticalSway *= 0.3;
			m_fADSPower = SWAY_ADS_POWER;
		}
		else
			m_fADSPower = 1;
		
		if (m_fTargetHorizontalSway > 0 && !SRGP_Utils.SRGP_IsInADS(m_weaponOwner))
			m_fCompensation = 0.5;
		else
			m_fCompensation = 1;
		
		m_fCurrentVerticalSway = Math.SmoothSpring(m_fCurrentVerticalSway, m_fTargetVerticalSway, SPRING_VELOCITY_VERTICAL, SWAY_SPRING_VERTICAL, SWAY_DAMPING_VERTICAL, timeSlice * SWAY_SPEED);
		m_fCurrentHorizontalSway = Math.SmoothSpring(m_fCurrentHorizontalSway, m_fTargetHorizontalSway, SPRING_VELOCITY_HORIZONTAL, SWAY_SPRING_HORIZONTAL, SWAY_DAMPING_HORIZONTAL, timeSlice * SWAY_SPEED);
		
		translation[0] = Math.Min(Math.Max(m_fCurrentHorizontalSway * OVERALL_INERTIA * m_fADSPower, -0.5), 0.5);
		translation[2] = Math.Min(Math.Max(m_fCurrentVerticalSway * OVERALL_INERTIA * -0.6 * m_fADSPower, -0.5), 0.5);
		
		rotation[2] = Math.Min(Math.Max(m_fCurrentHorizontalSway * OVERALL_INERTIA * m_fCompensation * 500 * INERTIA_ROLL, -90), 90);
	}
}