modded class SCR_ParticleContactComponent
{
	
	[Attribute("false", desc: "Is it muzzle blast", category: "sorrry")]
	bool m_bIsMuzzleBlast;
	[Attribute("1", desc: "1 - Tiny, 2 - small", category: "sorrry")]
	int m_iMuzzleBlastSize;
	
		override void EOnContact(IEntity owner, IEntity other, Contact contact)
	{
		// Get prefab data
		SCR_ParticleContactComponentClass prefabData = SCR_ParticleContactComponentClass.Cast(GetComponentData(owner));
		if (!prefabData)
		{
			ClearEventMask(owner, EntityEvent.CONTACT);
			return;
		}
				
		// Play sound
		PlaySound(owner, prefabData, contact);
		
		//Play VFX
		if (prefabData.m_bPlayParticle)
		{
			
			//Play game material effect	
			if (prefabData.m_iGameMaterialEffect == 1)
			{
				GameMaterial material = contact.Material2;
				ParticleEffectInfo effectInfo = material.GetParticleEffectInfo();
				
				if (effectInfo)
				{
					prefabData.m_Particle = effectInfo.GetVehicleDustResource(prefabData.m_iEffectIndex);	
				}
			} 
			else if (prefabData.m_iGameMaterialEffect == 2)
			{
				GameMaterial material = contact.Material2;
				ParticleEffectInfo effectInfo = material.GetParticleEffectInfo();
				
				if (effectInfo)
				{
					prefabData.m_Particle = effectInfo.GetBlastResource(prefabData.m_iEffectIndex);
					if (prefabData.m_Particle == "{2C7486D2E7E40717}Particles/Blasts/Blast_huge_default.ptc" && m_bIsMuzzleBlast)
					{
						switch (m_iMuzzleBlastSize)
						{
							case 1:
								prefabData.m_Particle = "{32FAF14467412366}Particles/Blasts/Blast_tiny_default.ptc";
								break;
							case 2:
								prefabData.m_Particle = "{6A99B14351DECD61}Particles/Blasts/Blast_small_default.ptc";
								break;
							default:
								prefabData.m_Particle = "{32FAF14467412366}Particles/Blasts/Blast_tiny_default.ptc";
								break;
						}
					}
				}
			}
			
			if (prefabData.m_Particle != string.Empty)
			{
				ParticleEffectEntitySpawnParams spawnParams = new ParticleEffectEntitySpawnParams();
				spawnParams.UseFrameEvent = true;
				
				if (prefabData.m_bParticleOriented)	
					Math3D.AnglesToMatrix(contact.Normal, spawnParams.Transform);

				spawnParams.Transform[3] = contact.Position;
				ParticleEffectEntity.SpawnParticleEffect(prefabData.m_Particle, spawnParams);
				
#ifdef ENABLE_DIAG
					if (DiagMenu.GetBool(SCR_DebugMenuID.DEBUGUI_PARTICLES_CONTACT_COMPONENT))
					{
						DebugTextWorldSpace.Create(GetOwner().GetWorld(), contact.Material2.GetName(), DebugTextFlags.CENTER, contact.Position[0], contact.Position[1], contact.Position[2]);
					}
#endif
			}
		}
				
		// Disable OnContact after the first contact
		if (prefabData.m_bFirstContactOnly)
			ClearEventMask(owner, EntityEvent.CONTACT);
	}


}