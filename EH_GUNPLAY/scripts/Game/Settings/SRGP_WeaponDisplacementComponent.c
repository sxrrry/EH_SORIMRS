class SRGP_WeaponDisplacementComponentClass : ScriptComponentClass
{
}

class SRGP_WeaponDisplacementComponent : ScriptComponent
{
    [RplProp(onRplName: "OnSettingsUpdated")]
    float ROTATION_X = 0;

    [RplProp(onRplName: "OnSettingsUpdated")]
    float ROTATION_Y = 0;

    [RplProp(onRplName: "OnSettingsUpdated")]
    float ROTATION_Z = -12;

    [RplProp(onRplName: "OnSettingsUpdated")]
    float OFFSET_X = -0.01;

    [RplProp(onRplName: "OnSettingsUpdated")]
    float OFFSET_Y = 0;

    [RplProp(onRplName: "OnSettingsUpdated")]
    float OFFSET_Z = 0;

    void RequestApplyValues(float rotX, float rotY, float rotZ, float offX, float offY, float offZ)
    {
        Rpc(RpcAsk_ApplyValues, rotX, rotY, rotZ, offX, offY, offZ);
    }

    [RplRpc(RplChannel.Reliable, RplRcver.Server)]
    void RpcAsk_ApplyValues(float rotX, float rotY, float rotZ, float offX, float offY, float offZ)
    {
        if (!Replication.IsServer())
            return;

        ROTATION_X = rotX;
        ROTATION_Y = rotY;
        ROTATION_Z = rotZ;
        OFFSET_X   = offX;
        OFFSET_Y   = offY;
        OFFSET_Z   = offZ;

        Replication.BumpMe();
    }

    void OnSettingsUpdated()
    {
    }
}