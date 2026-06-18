class SR_HandStaminaDisplay : SCR_InfoDisplayExtended
{
	[Attribute("{1FC52263BDF15CFB}UI/layouts/SRGP_HandsStaminaLayout.layout")]
	protected string m_sHandStaminaDisplayLayout;
	
	float m_fAlpha = 0;		// opacity
	float m_fCurrentStamina; // current hands stamina
	float m_fFadeoutTimer = 0;
	
	TextWidget m_wTDbgStamina;
	ProgressBarWidget m_wPBHandsStamina;
	
	bool m_bIsShown = false;
	
	protected void Init()
	{
		m_wTDbgStamina = TextWidget.Cast(m_wRoot.FindAnyWidget("m_wTDbgStamina"));
		if (!m_wTDbgStamina)
			return;
		m_wPBHandsStamina = ProgressBarWidget.Cast(m_wRoot.FindAnyWidget("m_wPBHandsStamina"));
		if (!m_wPBHandsStamina)
			return;
	}
	
	override protected void DisplayUpdate(IEntity owner, float timeSlice)
	{
		if (!m_wTDbgStamina || !m_wPBHandsStamina)
			Init();
		
		PlayerController pc = PlayerController.Cast(owner);
		if (!pc)
			return;
		SCR_ChimeraCharacter player = SCR_ChimeraCharacter.Cast(pc.GetControlledEntity());
		if (!player)
			return;
		SRGP_HandsStaminaCharacterComponent HSCC = SRGP_HandsStaminaCharacterComponent.Cast(player.FindComponent(SRGP_HandsStaminaCharacterComponent));
		if(!HSCC)
			return;
		m_fCurrentStamina = HSCC.GetStamina();
		
		m_wPBHandsStamina.SetCurrent(m_fCurrentStamina);
		m_wTDbgStamina.SetText(m_fCurrentStamina.ToString());
		
		//--------------------------------------------------------------------------
		// Color
		
		if (m_fCurrentStamina < 30)
			AnimateWidget.Color(m_wPBHandsStamina, Color.Red, 1);
		else if (m_fCurrentStamina >= 30 && m_fCurrentStamina < 60)
			AnimateWidget.Color(m_wPBHandsStamina, Color.Yellow, 1);
		else if (m_fCurrentStamina >= 60)
			AnimateWidget.Color(m_wPBHandsStamina, Color.White, 1);
		
		//--------------------------------------------------------------------------
		// Fadeout management
		if(HSCC.SRGP_IsInADS(player))
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
			m_fAlpha = Math.Lerp(m_fAlpha, 0.5, fadeSpeed * timeSlice);
			if (m_fAlpha > 0.99)
			{
				m_fAlpha = 1;
			}
		}
		
		widget.SetOpacity(m_fAlpha);
		
	}
}