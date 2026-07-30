// read projectile mass :)

modded class SCR_MuzzleEffectComponent
{
	protected float bulletMass;
	protected float bulletSpeed;
	
	override void OnFired(IEntity effectEntity, BaseMuzzleComponent muzzle, IEntity projectileEntity)
	{
        if (!projectileEntity)
            return;
        ProjectileMoveComponent move = ProjectileMoveComponent.Cast(projectileEntity.FindComponent(ProjectileMoveComponent));
        if (!move)
            return;
        BaseContainer container = move.GetComponentSource(projectileEntity);
        if (!container)
            return;
        container.Get("Mass", bulletMass);
		if (bulletMass <= 0)
			 bulletMass = 0.004;
		container.Get("InitSpeed", bulletSpeed);
		if (bulletSpeed <= 0)
			 bulletSpeed = 800;
		super.OnFired(effectEntity, muzzle, projectileEntity);
	}
	
	float GetBulletMass() {return bulletMass;}
	float GetBulletSpeed() {return bulletSpeed;}
}