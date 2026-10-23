class SRGP_CamShake
{
    static const float CAM_SHAKE_DURATION = 0.22;
    static const float SECOND_HARM = 0.30;  // 0 = smooth nod, 0.3 = visible dip, 0.5 = strong dip. this shit used to create more natural negative shake...
    static const float PITCH_BASE = 3.5;
    static const float YAW_BASE = 1;
    static const float ROLL_BASE = 1.8;

    static float s_fShakeAge = 999;
    static vector s_vShakeAmplitude;  // yaw, pitch, roll (degrees)

    static void ShakeCamera(float scale = 1.0)
    {
        float pitch = PITCH_BASE * Math.RandomFloat(0.85, 1.15);
        float yaw = YAW_BASE * Math.RandomFloat(-1.0, 1.0);
        float roll = ROLL_BASE * Math.RandomFloat(0.7, 1.3);

        float sign = 1.0;
        if (Math.RandomFloat01() < 0.5)
            sign = -1.0;
        roll *= sign;

        s_vShakeAmplitude = Vector(yaw * scale, pitch * scale, roll * scale);
        s_fShakeAge = 0;
    }

	// math shit
    static vector ShakeAngles(float scale = 1.0)
    {
        if (s_fShakeAge >= CAM_SHAKE_DURATION)
            return vector.Zero;

        float u = s_fShakeAge / CAM_SHAKE_DURATION;
        float s1 = Math.Sin(2.0 * Math.PI * u);
        float s2 = Math.Sin(4.0 * Math.PI * u);
        float wave = (1.0 - u) * (s1 - SECOND_HARM * s2);

		vector result = s_vShakeAmplitude * wave * scale;
        return result;
    }

    static void CountImpulseAge(float dt) {s_fShakeAge += dt;}

    static void RotateCamera(vector camMatrix[4], vector anglesDeg)
    {
        vector deltaBasis[3];
        Math3D.AnglesToMatrix(anglesDeg, deltaBasis);
        Math3D.MatrixMultiply3(camMatrix, deltaBasis, camMatrix);
    }
}