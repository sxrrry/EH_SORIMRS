class SR_HandStaminaDisplay : SCR_InfoDisplayExtended
{
	[Attribute("{1FC52263BDF15CFB}UI/layouts/SRGP_HandsStaminaLayout.layout")]
	protected string m_sHandStaminaDisplayLayout;
	[Attribute("0 0 100 1", uiwidget: UIWidgets.CurveDialog, desc: "Relation of stamina to indicator alpha", category: "Settings", params: "100 1 0 0")]
	protected ref Curve m_cStaminaToAlpha;
	
	float m_fAlpha = 0;		// opacity
	float m_fCurrentStamina; // current hands stamina
	float m_fCurrentBodyStaminaDebuffFactor;
	float m_fFadeoutTimer = 0;
	
	ImageWidget m_wDamagedArmsIcon;
	TextWidget m_wTDbgStamina;
	ProgressBarWidget m_wPBHandsStamina;
	ProgressBarWidget m_wPBHSBodyStaminaDebuff;
	
	bool m_bIsShown = false;
	
	protected void Init()
	{
		m_wTDbgStamina = TextWidget.Cast(m_wRoot.FindAnyWidget("m_wTDbgStamina"));
		if (!m_wTDbgStamina)
			return;
		m_wPBHandsStamina = ProgressBarWidget.Cast(m_wRoot.FindAnyWidget("m_wPBHandsStamina"));
		if (!m_wPBHandsStamina)
			return;
		m_wPBHSBodyStaminaDebuff = ProgressBarWidget.Cast(m_wRoot.FindAnyWidget("m_wPBHSBodyStaminaDebuff"));
		if (!m_wPBHSBodyStaminaDebuff)
			return;
	}
	
	override protected void DisplayUpdate(IEntity owner, float timeSlice)
	{
		if (!m_wTDbgStamina || !m_wPBHandsStamina || !m_wPBHSBodyStaminaDebuff)
			Init();
		
		PlayerController pc = PlayerController.Cast(owner);
		if (!pc)
			return;
		SCR_ChimeraCharacter player = SCR_ChimeraCharacter.Cast(pc.GetControlledEntity());
		if (!player)
			return;
		SRGP_HandsStaminaCharacterComponent HSCC = SRGP_HandsStaminaCharacterComponent.Cast(player.FindComponent(SRGP_HandsStaminaCharacterComponent));
		if (!HSCC)
			return;
		m_fCurrentStamina = HSCC.GetStamina();
		m_fCurrentBodyStaminaDebuffFactor = HSCC.SRGP_GetDebuffFactor();
		
		m_wPBHandsStamina.SetCurrent(m_fCurrentStamina);
		m_wPBHSBodyStaminaDebuff.SetCurrent(m_fCurrentBodyStaminaDebuffFactor);
		m_wTDbgStamina.SetText(m_fCurrentStamina.ToString());
		
		//--------------------------------------------------------------------------
		// Color
		
		if (m_fCurrentStamina < 15)
			AnimateWidget.Color(m_wPBHandsStamina, Color.Red, 2);
		else if (m_fCurrentStamina >= 15 && m_fCurrentStamina < 60)
			AnimateWidget.Color(m_wPBHandsStamina, Color.Yellow, 2);
		else if (m_fCurrentStamina >= 60)
			AnimateWidget.Color(m_wPBHandsStamina, Color.White, 2);
		/*
		if (m_fCurrentBodyStaminaDebuffFactor > 40)
			AnimateWidget.Color(m_wPBHSBodyStaminaDebuff, Color.Red, 2);
		else if (m_fCurrentBodyStaminaDebuffFactor <= 40 && m_fCurrentBodyStaminaDebuffFactor > 20)
			AnimateWidget.Color(m_wPBHSBodyStaminaDebuff, Color.Yellow, 2);
		else if (m_fCurrentBodyStaminaDebuffFactor <= 20)
			AnimateWidget.Color(m_wPBHSBodyStaminaDebuff, Color.White, 2);
		*/
		
		//--------------------------------------------------------------------------
		// Fadeout management
		if (SRGP_Utils.SRGP_IsInADS(player))
		{
			m_bIsShown = true;
			m_fFadeoutTimer = 0;
		}
		else
		{
			if (m_bIsShown)
				m_fFadeoutTimer = 3;
			else if (m_fFadeoutTimer > 0)
				m_fFadeoutTimer -= timeSlice;
			else
				m_fFadeoutTimer = 0;
			m_bIsShown = false;
		}
		
		FadeOpacity(m_wPBHandsStamina, timeSlice, 10);
		FadeOpacity(m_wPBHSBodyStaminaDebuff, timeSlice, 10);
	}
	
	protected void FadeOpacity(ProgressBarWidget widget, float timeSlice, float fadeSpeed)
	{
		if (!m_bIsShown && m_fFadeoutTimer <= 0.001)
		{
			m_fAlpha = Math.Lerp(m_fAlpha, 0, fadeSpeed * timeSlice);
			if (m_fAlpha < 0.01)
			{
				m_fAlpha = 0;
			}
		}
		else
		{
			float maxAlpha = LegacyCurve.Curve(
			ECurveType.CurveProperty2D,
			m_fCurrentStamina,
			m_cStaminaToAlpha)[1];
			m_fAlpha = Math.Lerp(m_fAlpha, maxAlpha, fadeSpeed * timeSlice);
			if (m_fAlpha > maxAlpha)
			{
				m_fAlpha = maxAlpha;
			}
		}
		widget.SetOpacity(m_fAlpha);
	}

}