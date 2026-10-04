class SRGP_WeaponDisplacementComponentClass : ScriptComponentClass
{
}

class SRGP_WeaponDisplacementComponent : ScriptComponent
{
    [RplProp(onRplName: "OnSettingsUpdated")] float ROTATION_X = 0;
    [RplProp(onRplName: "OnSettingsUpdated")] float ROTATION_Y = 0;
    [RplProp(onRplName: "OnSettingsUpdated")] float ROTATION_Z = 0;
    [RplProp(onRplName: "OnSettingsUpdated")] float OFFSET_X = 0;
    [RplProp(onRplName: "OnSettingsUpdated")] float OFFSET_Y = 0;
    [RplProp(onRplName: "OnSettingsUpdated")] float OFFSET_Z = 0;

    [RplProp(onRplName: "OnSettingsUpdated")] float PISTOL_ROTATION_X = 0;
    [RplProp(onRplName: "OnSettingsUpdated")] float PISTOL_ROTATION_Y = 0;
    [RplProp(onRplName: "OnSettingsUpdated")] float PISTOL_ROTATION_Z = 0;
    [RplProp(onRplName: "OnSettingsUpdated")] float PISTOL_OFFSET_X = 0;
    [RplProp(onRplName: "OnSettingsUpdated")] float PISTOL_OFFSET_Y = 0;
    [RplProp(onRplName: "OnSettingsUpdated")] float PISTOL_OFFSET_Z = 0;

    [RplProp(onRplName: "OnSettingsUpdated")] bool ENABLE_PISTOL_SETTINGS = false;

    void RequestApplyValues(
        float rotX, float rotY, float rotZ, float offX, float offY, float offZ,
        float pRotX, float pRotY, float pRotZ, float pOffX, float pOffY, float pOffZ,
        bool enablePistol
    )
    {
        Rpc(RpcAsk_ApplyRifle, rotX, rotY, rotZ, offX, offY, offZ, enablePistol);
        Rpc(RpcAsk_ApplyPistol, pRotX, pRotY, pRotZ, pOffX, pOffY, pOffZ);
    }

    [RplRpc(RplChannel.Reliable, RplRcver.Server)]
    void RpcAsk_ApplyRifle(float rotX, float rotY, float rotZ, float offX, float offY, float offZ, bool enablePistol)
    {
        if (!Replication.IsServer())
            return;

        ROTATION_X = rotX;
        ROTATION_Y = rotY;
        ROTATION_Z = rotZ;
        OFFSET_X   = offX;
        OFFSET_Y   = offY;
        OFFSET_Z   = offZ;

        ENABLE_PISTOL_SETTINGS = enablePistol;

        Replication.BumpMe();
    }

    [RplRpc(RplChannel.Reliable, RplRcver.Server)]
    void RpcAsk_ApplyPistol(float rotX, float rotY, float rotZ, float offX, float offY, float offZ)
    {
        if (!Replication.IsServer())
            return;

        PISTOL_ROTATION_X = rotX;
        PISTOL_ROTATION_Y = rotY;
        PISTOL_ROTATION_Z = rotZ;
        PISTOL_OFFSET_X   = offX;
        PISTOL_OFFSET_Y   = offY;
        PISTOL_OFFSET_Z   = offZ;

        Replication.BumpMe();
    }

    void OnSettingsUpdated()
    {
    }
}