// Thanks to WCS tonimontana for this code i slightly changed for my needs. All credits going to tonimontana and WCS team!
class SRY_ExtendedViewRangeSystem : GameSystem
{
	protected static const float CHECK_INTERVAL = 10.0;
	protected float m_fdesiredDistance = 4000;
	protected bool m_bEnable = true;
	
	protected IEntity m_pLocalPlayer;
	protected vector m_vPlayerOrigin;
	protected float m_fTimer;
	protected float m_fCharacterDistanceSq;
	protected ref set<IEntity> m_aPreviousEntities = new set<IEntity>();
	protected ref set<IEntity> m_aCurrentEntities = new set<IEntity>();

	//------------------------------------------------------------------------------------------------
	override static void InitInfo(WorldSystemInfo outInfo)
	{
		outInfo
			.SetAbstract(false)
			.SetUnique(true)
			.SetLocation(WorldSystemLocation.Client)
			.ClearPoints()
			.AddPoint(WorldSystemPoint.FixedFrame);
	}
	//------------------------------------------------------------------------------------------------
	override event protected void OnUpdatePoint(WorldUpdatePointArgs args)
	{
		PlayerController playerController = GetGame().GetPlayerController();
		if (!playerController)
			return;

		m_fTimer += args.GetTimeSliceSeconds();

		if (m_fTimer < CHECK_INTERVAL)
			return;

		m_fTimer = 0;

		m_pLocalPlayer = playerController.GetControlledEntity();
		if (!m_pLocalPlayer)
			return;

		UpdateNearEntities();
	}
	//------------------------------------------------------------------------------------------------
	protected void UpdateNearEntities()
	{
		//Clear enties left is setting is disabled
		if(!m_bEnable  && !m_aPreviousEntities.IsEmpty())
		{
			// Revert flags on all entities 
			foreach (IEntity prevEntity : m_aPreviousEntities)
			{
				if (prevEntity)
					RevertEntityProperties(prevEntity);
			}
		m_aPreviousEntities.Clear();
		return;
		}
		
		if(!m_bEnable)
			return;
		
		float viewDistance = GetGame().GetViewDistance();
		float userViewDistance = Math.Min(m_fdesiredDistance,viewDistance);
		float vehicleRange = userViewDistance;
		float characterRange = userViewDistance;

		m_vPlayerOrigin = m_pLocalPlayer.GetOrigin();
		m_fCharacterDistanceSq = characterRange * characterRange;
		m_aCurrentEntities.Clear();

		// Query at the larger vehicle range, filter characters by distance in OnEntityFound
		GetGame().GetWorld().QueryEntitiesBySphere(m_vPlayerOrigin, vehicleRange, OnEntityFound, FilterEntities, EQueryEntitiesFlags.DYNAMIC);

		// Revert flags on entities that left the range
		foreach (IEntity prevEntity : m_aPreviousEntities)
		{
			if (prevEntity && !m_aCurrentEntities.Contains(prevEntity))
				RevertEntityProperties(prevEntity);
		}

		// Swap sets for next cycle
		set<IEntity> temp = m_aPreviousEntities;
		m_aPreviousEntities = m_aCurrentEntities;
		m_aCurrentEntities = temp;
	}
	//------------------------------------------------------------------------------------------------
	protected bool FilterEntities(IEntity ent)
	{
		if (ent == m_pLocalPlayer)
			return false;

		if (ent.FindComponent(VehicleBaseSimulation))
			return true;

		if (SCR_ChimeraCharacter.Cast(ent) != null)
			return true;

		return false;
	}
	//------------------------------------------------------------------------------------------------
	protected bool OnEntityFound(IEntity ent)
	{
		// Characters use the shorter range
		if (SCR_ChimeraCharacter.Cast(ent) != null)
		{
			if (vector.DistanceSq(ent.GetOrigin(), m_vPlayerOrigin) > m_fCharacterDistanceSq)
				return true;
		}

		ent.SetVComponentFlags(VCFlags.NOFILTER);
		ent.SetFlags(EntityFlags.VISIBLE | EntityFlags.TRACEABLE);
		m_aCurrentEntities.Insert(ent);
		return true;
	}
	//------------------------------------------------------------------------------------------------
	protected void RevertEntityProperties(IEntity entity)
	{
		entity.ClearFlags(EntityFlags.TRACEABLE);
	}

}