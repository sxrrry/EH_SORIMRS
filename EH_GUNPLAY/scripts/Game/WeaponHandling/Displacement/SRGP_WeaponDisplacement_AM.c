class SRGP_WeaponDisplacement_AM : ScriptedWeaponAimModifier
{
    IEntity m_weaponOwner;
    SRGP_WeaponDisplacementComponent m_SettingsComp;
	
	[Attribute("False", uiwidget: UIWidgets.CheckBox, desc: "Use displacement for handguns", category: "Settings")]
	bool IS_HANDGUN;

    float SPRING_VELOCITY = 0;
    float m_fCurrentMult;
    float m_fTargetMult;

    bool m_bSyncSent = false;

    override protected void OnActivated(IEntity weaponOwner)
    {
        m_weaponOwner = weaponOwner;

        m_SettingsComp = SRGP_WeaponDisplacementComponent.Cast(
            weaponOwner.FindComponent(SRGP_WeaponDisplacementComponent)
        );
    }
	
	protected bool IsPlayerCharacter(IEntity entity)
    {
        PlayerManager pm = GetGame().GetPlayerManager();
        if (!pm)
            return false;

        return pm.GetPlayerIdFromControlledEntity(entity) >= 0;
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
	        persistence.ROTATION_X,
	        persistence.ROTATION_Y,
	        persistence.ROTATION_Z,
	        persistence.OFFSET_X,
	        persistence.OFFSET_Y,
	        persistence.OFFSET_Z
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
        if (!m_weaponOwner)
            return;
		if (!IsPlayerCharacter(m_weaponOwner))
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

        float rotX, rotY, rotZ, offX, offY, offZ;

        SCR_PlayerController pc = SCR_PlayerController.Cast(GetGame().GetPlayerController());
        bool isOwner = (pc && pc.GetControlledEntity() == m_weaponOwner);

        if (isOwner)
        {
            BaseContainer s = GetGame().GetGameUserSettings().GetModule("SRGP_WeaponDisplacementSettings");
            if (!s)
                return;

            s.Get("ROTATION_X", rotX);
            s.Get("ROTATION_Y", rotY);
            s.Get("ROTATION_Z", rotZ);
            s.Get("OFFSET_X", offX);
            s.Get("OFFSET_Y", offY);
            s.Get("OFFSET_Z", offZ);
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

        if (SRGP_Utils.SRGP_IsInADS(m_weaponOwner))
            m_fTargetMult = 0;
        else
            m_fTargetMult = 1;
		if (SRGP_Utils.SRGP_GetStance(m_weaponOwner) == 2)
			m_fTargetMult *= 0;
		if (SRGP_Utils.SRGP_IsWeaponDeployed(m_weaponOwner) == 1 || SRGP_Utils.SRGP_IsWeaponDeployed(m_weaponOwner) == 2)
			m_fTargetMult *= 0;
			

        m_fCurrentMult = Math.SmoothSpring(
            m_fCurrentMult, m_fTargetMult,
            SPRING_VELOCITY, 0.1, 0.5, timeSlice * 35
        );

        translation[0] = offX * m_fCurrentMult;
        translation[1] = offY * m_fCurrentMult;
        translation[2] = offZ * m_fCurrentMult;

        rotation[0] = rotX * m_fCurrentMult;
        rotation[1] = rotY * m_fCurrentMult;
        rotation[2] = rotZ * m_fCurrentMult;
    }
}