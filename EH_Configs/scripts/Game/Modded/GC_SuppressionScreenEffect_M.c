modded class GC_SuppressionScreenEffect
{
	override void Flinch()
	{
		if(!m_bIsEnabled)
			return;
			
		m_wFlinch.SetMaskProgress(0);
		
		AnimateWidget.StopAllAnimations(m_wFlinch);
		AnimateWidget.AlphaMask(m_wFlinch, 0.6, 8)
			.SetCurve(EAnimationCurve.EASE_IN_CUBIC);
		
		GetGame().GetCallqueue().Remove(ClearFlinch);
		GetGame().GetCallqueue().CallLater(ClearFlinch, 120, false);
	}
	
	override void ClearFlinch()
	{
		GetGame().GetCallqueue().Remove(ClearFlinch);
		
		if(!m_wFlinch)
			return;
		
		AnimateWidget.StopAllAnimations(m_wFlinch);
		AnimateWidget.AlphaMask(m_wFlinch, 0, 8)
			.SetCurve(EAnimationCurve.EASE_OUT_CUBIC);	
	}




}