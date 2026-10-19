modded class CharacterCamera1stPerson
{
    override void OnUpdate(float pDt, out ScriptedCameraItemResult pOutResult)
    {
        super.OnUpdate(pDt, pOutResult);
        
        if (!m_OwnerCharacter || !SRGP_Utils.SRGP_IsLocalPlayerEntity(m_OwnerCharacter))
            return;
        
        SRGP_CamShake.Tick(pDt);
        SRGP_CamShake.Rotate(pOutResult.m_CameraTM, SRGP_CamShake.KickAngles(1));
    }
}

modded class CharacterCameraADS
{
    override void OnUpdate(float pDt, out ScriptedCameraItemResult pOutResult)
    {
        super.OnUpdate(pDt, pOutResult);
        
        if (!m_OwnerCharacter || !SRGP_Utils.SRGP_IsLocalPlayerEntity(m_OwnerCharacter))
            return;
        
        SRGP_CamShake.Tick(pDt);
        SRGP_CamShake.Rotate(pOutResult.m_CameraTM, SRGP_CamShake.KickAngles(1));
    }
}