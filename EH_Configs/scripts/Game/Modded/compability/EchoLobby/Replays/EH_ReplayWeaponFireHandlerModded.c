#ifdef ECHO_LOBBY_LOADED
modded class EH_ReplayExplosiveEffect 
{
    override protected bool IsSkippable(array<BaseProjectileEffect> triggerEffects)
    {
        int effectsCount = triggerEffects.Count();
        if (effectsCount == 1 && EH_ReplayExplosiveEffect.Cast(triggerEffects[0]))
            return true;
        
        if (effectsCount > 2)
            return false; 
        
        foreach (BaseProjectileEffect effect : triggerEffects)
        {
            GC_SuppressionEffect suppressionEffect = GC_SuppressionEffect.Cast(effect);
            EH_ReplayExplosiveEffect replayEffect = EH_ReplayExplosiveEffect.Cast(effect);
            if (!suppressionEffect && !replayEffect)
                return false;
        }
        
        return true;
    }
};
#endif