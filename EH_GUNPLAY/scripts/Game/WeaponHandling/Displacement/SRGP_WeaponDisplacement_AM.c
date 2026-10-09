class SRGP_WeaponDisplacement_AM : ScriptedWeaponAimModifier
{
    [Attribute("False", uiwidget: UIWidgets.CheckBox, desc: "Use displacement for handguns", category: "Settings")]
    private bool IS_HANDGUN;

    private IEntity m_weaponOwner;
    private SRGP_WeaponDisplacementComponent m_SettingsComp;

    private float SPRING_VELOCITY = 0;
    private float m_fCurrentMult;
    private float m_fTargetMult;

	private bool m_bSyncSent = false;
	
	private float m_fControlCheckTDelta;
	private bool m_bIsPlayer;

    override protected void OnActivated(IEntity weaponOwner)
    {
        m_weaponOwner = weaponOwner;
		m_fControlCheckTDelta = 1;
		m_bIsPlayer = false;

        m_SettingsComp = SRGP_WeaponDisplacementComponent.Cast(
            weaponOwner.FindComponent(SRGP_WeaponDisplacementComponent)
        );
    }

    protected void LoadAndPushPersistedSettings()
    {
        if (!m_SettingsComp)
            return;

        SCR_PlayerController pc = SCR_PlayerController.Cast(GetGame().GetPlayerController());
        if (!pc || pc.GetControlledEntity() != m_weaponOwner)
            return;

        SRGP_WeaponDisplacementPersistence persistence = new SRGP_WeaponDisplacementPersistence();
        persistence.LoadFromFileOrDefaults();
        persistence.ApplyToGameSettings();

        m_SettingsComp.RequestApplyValues(
            persistence.ROTATION_X, persistence.ROTATION_Y, persistence.ROTATION_Z,
            persistence.OFFSET_X, persistence.OFFSET_Y, persistence.OFFSET_Z,
            persistence.PISTOL_ROTATION_X, persistence.PISTOL_ROTATION_Y, persistence.PISTOL_ROTATION_Z,
            persistence.PISTOL_OFFSET_X, persistence.PISTOL_OFFSET_Y, persistence.PISTOL_OFFSET_Z,
            persistence.ENABLE_PISTOL_SETTINGS
        );
    }

    override void OnCalculate(
        IEntity owner,
        WeaponAimModifierContext context,
        float timeSlice,
        out vector translation,
        out vector rotation,
        out vector turnOffset
    )
    {
		timeSlice = Math.Min(timeSlice, 0.033);
		translation = vector.Zero;
		rotation = vector.Zero;
		turnOffset = vector.Zero;
		
        if (!m_weaponOwner)
			return;
		if (m_fControlCheckTDelta >= 0.5)
		{
			m_fControlCheckTDelta = 0;
			m_bIsPlayer = SRGP_Utils.IsPlayerCharacter(m_weaponOwner);
		}
		m_fControlCheckTDelta += timeSlice;
		if (!m_bIsPlayer)
		    return;

        if (!m_bSyncSent)
        {
            SCR_PlayerController pc0 = SCR_PlayerController.Cast(GetGame().GetPlayerController());
            if (pc0 && pc0.GetControlledEntity() == m_weaponOwner)
            {
                LoadAndPushPersistedSettings();
                m_bSyncSent = true;
            }
        }

        if (!m_SettingsComp)
        {
            m_SettingsComp = SRGP_WeaponDisplacementComponent.Cast(
                m_weaponOwner.FindComponent(SRGP_WeaponDisplacementComponent)
            );
            if (!m_SettingsComp)
                return;
        }

        SCR_PlayerController pc = SCR_PlayerController.Cast(GetGame().GetPlayerController());
        bool isOwner = (pc && pc.GetControlledEntity() == m_weaponOwner);

        bool enablePistol = false;
        if (isOwner)
        {
            BaseContainer s = GetGame().GetGameUserSettings().GetModule("SRGP_WeaponDisplacementSettings");
            if (s)
                s.Get("ENABLE_PISTOL_SETTINGS", enablePistol);
        }
        else
        {
            enablePistol = m_SettingsComp.ENABLE_PISTOL_SETTINGS;
        }

        bool usePistol = IS_HANDGUN && enablePistol;

        float rotX, rotY, rotZ, offX, offY, offZ;

        if (isOwner)
        {
            BaseContainer s = GetGame().GetGameUserSettings().GetModule("SRGP_WeaponDisplacementSettings");
            if (!s)
                return;

            if (usePistol)
            {
                s.Get("PISTOL_ROTATION_X", rotX);
                s.Get("PISTOL_ROTATION_Y", rotY);
                s.Get("PISTOL_ROTATION_Z", rotZ);
                s.Get("PISTOL_OFFSET_X", offX);
                s.Get("PISTOL_OFFSET_Y", offY);
                s.Get("PISTOL_OFFSET_Z", offZ);
            }
            else
            {
                s.Get("ROTATION_X", rotX);
                s.Get("ROTATION_Y", rotY);
                s.Get("ROTATION_Z", rotZ);
                s.Get("OFFSET_X", offX);
                s.Get("OFFSET_Y", offY);
                s.Get("OFFSET_Z", offZ);
            }
        }
        else
        {
            if (usePistol)
            {
                rotX = m_SettingsComp.PISTOL_ROTATION_X;
                rotY = m_SettingsComp.PISTOL_ROTATION_Y;
                rotZ = m_SettingsComp.PISTOL_ROTATION_Z;
                offX = m_SettingsComp.PISTOL_OFFSET_X;
                offY = m_SettingsComp.PISTOL_OFFSET_Y;
                offZ = m_SettingsComp.PISTOL_OFFSET_Z;
            }
            else
            {
                rotX = m_SettingsComp.ROTATION_X;
                rotY = m_SettingsComp.ROTATION_Y;
                rotZ = m_SettingsComp.ROTATION_Z;
                offX = m_SettingsComp.OFFSET_X;
                offY = m_SettingsComp.OFFSET_Y;
                offZ = m_SettingsComp.OFFSET_Z;
            }
        }

        if (SRGP_Utils.SRGP_IsInADS(m_weaponOwner))
            m_fTargetMult = 0;
        else
            m_fTargetMult = 1;
		if (SRGP_Utils.SRGP_IsWeaponDeployed(m_weaponOwner) > 0)
			m_fTargetMult *= 0;
		if (SRGP_Utils.SRGP_GetStance(m_weaponOwner) == 2)
			m_fTargetMult *= 0;

		float springSpeed = 35;
		
		// fix stupid pistol ads glitch
		if (IS_HANDGUN && SRGP_Utils.SRGP_IsInADS(m_weaponOwner))
			springSpeed = 100;
		else
			springSpeed = 35;
		
        m_fCurrentMult = Math.SmoothSpring(
            m_fCurrentMult, m_fTargetMult,
            SPRING_VELOCITY, 0.1, 0.5, timeSlice * springSpeed
        );
		
        translation[0] = offX * m_fCurrentMult;
        translation[1] = offY * m_fCurrentMult;
        translation[2] = offZ * m_fCurrentMult;

        rotation[0] = rotX * m_fCurrentMult;
        rotation[1] = rotY * m_fCurrentMult;
        rotation[2] = rotZ * m_fCurrentMult;
    }
}