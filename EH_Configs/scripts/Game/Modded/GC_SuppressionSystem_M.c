modded class GC_SuppressionSystem : GameSystem
{
	
	[Attribute("2.0", UIWidgets.Auto,
	"How much we should cut suppression effects range when in cover by creating fake bigger distance? (distance * this = 3m act will be 6 if set to 2.0)",
	params: "0 10.0")]
	protected float m_CoverRangeCut;
	
	override void OnControlledEntityChanged(IEntity from, IEntity to)
	{
		SetEnabled(true);
		Print("OnControlledEntityChanged - Flush suppression");
		protected float m_fSuppression = 0;
	}

	override float ExplosionScreenShake(float impulse)
	{
		if (!m_bExplosionScreenShakeEnabled)
				return 0;
		
		float minImpulse = Math.Max(m_fExplosionScreenShakeMinImpulse, 0.0);
		float maxImpulse = Math.Max(m_fExplosionScreenShakeMaxImpulse, minImpulse + 0.001);
	
		if (impulse <= 0 || impulse < minImpulse)
			return 0;
	
		float normalized = Math.InverseLerp(minImpulse, maxImpulse, impulse);
		normalized = Math.Clamp(normalized, 0.0, 1.0);
	
		float amount = Math.Lerp(0.0, m_fExplosionScreenShakeMaxAmount, normalized);
		amount = amount * m_fExplosionScreenShakeMultiplier;
		
		float sustainTime = Math.Lerp(0.1, m_fExplosionScreenShakeMaxSustainTime, normalized);
		float outTime = Math.Lerp(0.0, m_fExplosionScreenShakeMaxOutTime, normalized);

		//=================>
		SCR_ChimeraCharacter player = SCR_ChimeraCharacter.Cast(GetGame().GetPlayerController().GetControlledEntity());
		if (!player)
			return 0;

		if (player.IsInVehicle())
		{
			amount *= 0.5;
			
			if (amount > 6)
				amount = 6;
			
			if (player.IsInVehicleADS() && amount > 1)
				amount = 1;
		}
		//<==================
		
		ScreenShake(amount, 0.01, sustainTime, outTime);
		
		PrintFormat("GC | ExplosionScreenShake: impulse:%1, amount:%2, sustainTime:%3", impulse, amount, sustainTime);
		
		return amount;

	}
	
	override void HandleBulletImpact(IEntity bullet, vector transform[3], float multiplier, float distance, float speed)
	{
		if (!m_bIsEnabled)
			return;
		
		SCR_ChimeraCharacter player = SCR_ChimeraCharacter.Cast(GetGame().GetPlayerController().GetControlledEntity());
		if (!player)
			return;
		
		bool isInCover = IsInCover(transform[1]);
		
		if(player.IsInVehicle() && !isInCover)
		{
			if(!IsLineOfSight(transform, null))
				return;
		}
		
		float multi = m_fHitSuppressionMultiplier;
		if(isInCover)
		{
			multi -= m_fCoverMultiplier;
			// SRY MULTIPLY
			distance = distance * m_CoverRangeCut;	
		}
		
		multi *= multiplier;
		
		// lock max supp for veh
		//=================>
		if (player.IsInVehicle() && m_fSuppression > 0.4)
			return;
		//<==================
		
		float suppression = GetBulletSuppression(bullet, distance, multi);
		AddSuppression(suppression);
	}
	
	override protected void UpdateProjectiles()
	{
		SCR_ChimeraCharacter player = SCR_ChimeraCharacter.Cast(GetGame().GetPlayerController().GetControlledEntity());
		if (!player)
			return;
	
		vector playerEyePos = player.EyePosition();
	
		float maxRangeSq = m_fMaxRange * m_fMaxRange;
		float flinchRangeSq = m_fFlinchRange * m_fFlinchRange;
	
		bool isInVehicle = player.IsInVehicle();
	
		for (int i = m_aProjectiles.Count() - 1; i >= 0; i--)
		{
			GC_ProjectileComponent projectile = m_aProjectiles[i];
	
			bool removeProjectile = false;
	
			IEntity owner = projectile.GetOwner();
			if (!owner)
				removeProjectile = true;
			else
			{
				ProjectileMoveComponent move = ProjectileMoveComponent.Cast(owner.FindComponent(ProjectileMoveComponent));
				if (!move)
					removeProjectile = true;
				else
				{
					vector projPos = owner.GetOrigin();

					float approach = vector.Dot(playerEyePos - projPos, move.GetVelocity());
	
					if (approach > 0)
					{
						projectile.position = projPos;
					}
					else
					{
						removeProjectile = true;

						vector flybyPos = GetFlybyPoint(projPos, projectile.position, playerEyePos);
						float distSq = vector.DistanceSq(flybyPos, playerEyePos);
	
						bool inSuppRange = distSq <= maxRangeSq;
	
						if (inSuppRange)
						{
							bool doEffects = true;
	
							if (isInVehicle)
							{
								if (!IsLineOfSight(flybyPos))
									doEffects = false;
							}
	
							if (doEffects)
							{
								
								float dist = Math.Sqrt(distSq);
								
								if (inSuppRange)
								{
									bool isInCover = IsInCover(projectile);
	
									float multi = 1.0;
									if (isInCover)
									{
										multi = Math.Clamp(multi - m_fCoverMultiplier, 0.0, 1.0);
										// SRY MULTIPLY
										dist = dist * m_CoverRangeCut;
									}
										
										
									float suppression = GetBulletSuppression(projectile, dist, multi);
									AddSuppression(suppression);
								}
								
								
								// SRY MULTIPLY
								if (IsInCover(projectile))
									distSq = dist * m_CoverRangeCut;
								
								if (distSq <= flinchRangeSq)
									Flinch();
							}
						}
					}
				}
			}
	
			if (removeProjectile)
				m_aProjectiles.Remove(i);
		}
	}
	
}