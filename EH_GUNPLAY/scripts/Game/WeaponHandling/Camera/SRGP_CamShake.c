class SRGP_CamShake
{
    static const float KICK_DURATION = 0.22;
    static const float SECOND_HARM   = 0.30;  // 0 = smooth nod, 0.3 = visible dip, 0.5 = strong dip. this shit used to create more natural negative shake...
    static const float PITCH_BASE    = 3.5;
    static const float YAW_BASE      = 1;
    static const float ROLL_BASE     = 1.8;

    static float  s_fKickAge = 999;
    static vector s_vKickAmp;  // yaw, pitch, roll (degrees)

    static void Kick(float scale = 1.0)
    {
        float pitch = PITCH_BASE * Math.RandomFloat(0.85, 1.15);
        float yaw   = YAW_BASE   * Math.RandomFloat(-1.0, 1.0);
        float roll  = ROLL_BASE  * Math.RandomFloat(0.7, 1.3);
        if (Math.RandomFloat01() < 0.5)
            roll = -roll;

        s_vKickAmp = Vector(yaw * scale, pitch * scale, roll * scale);
        s_fKickAge = 0;
    }

	// math shit
    static vector KickAngles(float scale = 1.0)
    {
        if (s_fKickAge >= KICK_DURATION)
            return vector.Zero;

        float u = s_fKickAge / KICK_DURATION;
        float s1 = Math.Sin(2.0 * Math.PI * u);
        float s2 = Math.Sin(4.0 * Math.PI * u);
        float wave = (1.0 - u) * (s1 - SECOND_HARM * s2);

        return s_vKickAmp * wave * scale;
    }

    static void Tick(float dt) {s_fKickAge += dt;}

    static void Rotate(vector tm[4], vector ypr)
    {
        vector rot[3];
        Math3D.AnglesToMatrix(ypr, rot);
        Math3D.MatrixMultiply3(tm, rot, tm);
    }
}