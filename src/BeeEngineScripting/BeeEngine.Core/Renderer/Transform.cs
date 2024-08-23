using BeeEngine.Math;

namespace BeeEngine.Renderer;

/// <summary>
/// Represents a 3D transformation consisting of translation, rotation, and scale.
/// </summary>
public struct Transform
{
    /// <summary>
    /// The translation vector of the transform.
    /// </summary>
    public Vector3 Translation;

    /// <summary>
    /// The rotation vector of the transform.
    /// </summary>
    public Vector3 Rotation;

    /// <summary>
    /// The scale vector of the transform.
    /// </summary>
    public Vector3 Scale;

    /// <summary>
    /// Gets the transformation matrix representing this transform.
    /// </summary>
    public Matrix4 Matrix => Matrix4.CreateTransform(Translation, Rotation, Scale);

    /// <summary>
    /// Initializes a new instance of the <see cref="Transform"/> struct with default values.
    /// </summary>
    public Transform()
    {
        Translation = Vector3.Zero;
        Rotation = Vector3.Zero;
        Scale = Vector3.One;
    }
    /// <summary>
    /// Initializes a new instance of the <see cref="Transform"/> struct from a given transformation matrix.
    /// </summary>
    /// <param name="transform">The transformation matrix to extract the transform from.</param>
    /// <remarks>
    /// This constructor extracts the translation, rotation, and scale components from the given transformation matrix.
    /// It handles perspective and coordinate system flips.
    /// </remarks>
    public Transform(Matrix4 transform)
    {
        // Check for perspective
        if (MathU.Abs(transform.M33) < 1e-6)
        {
            Translation = Vector3.Zero;
            Rotation = Vector3.Zero;
            Scale = Vector3.One;
            return;
        }

        // Clear perspective partition
        if (MathU.Abs(transform.M14) > 1e-6 ||
            MathU.Abs(transform.M24) > 1e-6 ||
            MathU.Abs(transform.M34) > 1e-6)
        {
            transform.M14 = 0;
            transform.M24 = 0;
            transform.M34 = 0;
            transform.M44 = 1;
        }

        // Extract translation
        Vector3 translation = new Vector3(transform.M14, transform.M24, transform.M34);
        transform.M14 = 0;
        transform.M24 = 0;
        transform.M34 = 0;
        transform.M44 = 1;

        // Extract scale and shear
        Vector3 scale = new Vector3();
        Vector3 row0 = new Vector3(transform.M11, transform.M21, transform.M31);
        Vector3 row1 = new Vector3(transform.M12, transform.M22, transform.M32);
        Vector3 row2 = new Vector3(transform.M13, transform.M23, transform.M33);

        scale.X = row0.Length;
        row0.Normalize();
        scale.Y = row1.Length;
        row1.Normalize();
        scale.Z = row2.Length;
        row2.Normalize();

        // Determine if there is a coordinate system flip
        Vector3 crossProduct = Vector3.Cross(row1, row2);
        if (Vector3.Dot(row0, crossProduct) < 0)
        {
            scale *= -1;
            row0 *= -1;
            row1 *= -1;
            row2 *= -1;
        }

        // Extract rotation
        Vector3 rotation = new Vector3();
        rotation.Y = (float)MathU.Asin(-row0.Z);
        if (MathU.Cos(rotation.Y) != 0)
        {
            rotation.X = (float)MathU.Atan2(row1.Z, row2.Z);
            rotation.Z = (float)MathU.Atan2(row0.Y, row0.X);
        }
        else
        {
            rotation.X = (float)MathU.Atan2(-row2.X, row1.Y);
            rotation.Z = 0;
        }
        Translation = translation;
        Rotation = rotation;
        Scale = scale;
    }
}