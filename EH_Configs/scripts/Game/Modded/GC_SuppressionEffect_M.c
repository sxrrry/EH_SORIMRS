modded class GC_SuppressionEffect
{
	override protected void HandleBullet(IEntity source, vector transform[3], Instigator instigator, float speed)
	{
		PlayerController pc = GetGame().GetPlayerController();
		if (!pc)
			return;
		
		SCR_ChimeraCharacter chimera = SCR_ChimeraCharacter.Cast(pc.GetControlledEntity());
		if (!chimera)
			return;
		
		IEntity shooter = instigator.GetInstigatorEntity();
		if (shooter == chimera)
			return;
		
		GC_SuppressionSystem suppr = GC_SuppressionSystem.GetInstance();
		if (!suppr)
			return;
		
		bool isFriendly = false;
		if (shooter)
		{
			FactionAffiliationComponent playerFac = FactionAffiliationComponent.Cast(
				chimera.FindComponent(FactionAffiliationComponent));
			FactionAffiliationComponent shooterFac = FactionAffiliationComponent.Cast(
				shooter.FindComponent(FactionAffiliationComponent));
			
			if (playerFac && shooterFac)
			{
				if (playerFac.GetAffiliatedFaction() == shooterFac.GetAffiliatedFaction())
					isFriendly = true;
			}
		}
		
		if (isFriendly)
		{
			if (suppr.GetIgnoreFriendlyImpactSuppression())
				return;
			
			vector shooterPos = shooter.GetOrigin();
			SCR_ChimeraCharacter shooterChar = SCR_ChimeraCharacter.Cast(shooter);
			if (shooterChar)
				shooterPos = shooterChar.EyePosition();
			
			float shooterDistance = vector.Distance(shooterPos, chimera.EyePosition());
			if (shooterDistance < suppr.GetMinRange())
				return;
		}
		
		float distance = vector.Distance(transform[0], chimera.EyePosition());
		if (distance > suppr.GetMaxRange())
			return;

		suppr.HandleBulletImpact(source, transform, m_fMultiplier, distance, speed);
	}
}