using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Net;
using System.Net.Sockets;
using System.Text;
using UnityEngine;

[RequireComponent(typeof(ParticleSystem))]
public class LidarReceiver : MonoBehaviour 
{
    [Header("Network Settings")]
    public int port = 5005;

    [Header("Scaling & Visuals")]
    public float distanceScale = 5.0f;
    public float particleSize = 0.4f;
    public Color particleColor = Color.cyan;

    [Header("3D Vertical Wall Settings")]
    [Tooltip("Total vertical height of the wall (in meters).")]
    public float wallHeight = 4.0f; 
    
    [Tooltip("Number of vertical particles stacked at each LiDAR point to visualize the wall height.")]
    public int verticalStackCount = 5; 
    
    public float wallColliderRadius = 0.4f;

    private UdpClient udpClient;
    private ParticleSystem particleSys;
    private ParticleSystem.Particle[] particles;
    private readonly ConcurrentQueue<Vector3[]> scanQueue = new ConcurrentQueue<Vector3[]>();

    private GameObject colliderHolder;
    private readonly List<CapsuleCollider> colliderPool = new List<CapsuleCollider>();

    [System.Serializable]
    public class PointData { public float a; public float d; }

    [System.Serializable]
    public class ScanPacket { public PointData[] pts; }

    void Start() 
    {
        particleSys = GetComponent<ParticleSystem>();
        SetupParticleSystem();

        colliderHolder = new GameObject("LiDAR_3D_Vertical_Walls");
        colliderHolder.transform.SetParent(transform, false);

        try 
        {
            udpClient = new UdpClient(port);
            udpClient.BeginReceive(OnDataReceived, null);
            Debug.Log($"<color=green>[LIDAR] Listening on Port {port}</color>");
        }
        catch (Exception e)
        {
            Debug.LogError($"[LIDAR] Bind Error: {e.Message}");
        }
    }

    void Update()
    {
        if (scanQueue.TryDequeue(out Vector3[] latestPositions))
        {
            RenderExtrudedParticles(latestPositions);
            UpdateCapsuleColliders(latestPositions);
        }
    }

    // 1. Visually extrude particles vertically so you can see the 3D wall
    private void RenderExtrudedParticles(Vector3[] positions)
    {
        int totalParticlesNeeded = positions.Length * verticalStackCount;

        if (particles == null || particles.Length < totalParticlesNeeded)
        {
            particles = new ParticleSystem.Particle[totalParticlesNeeded];
        }

        Vector3 origin = transform.position;
        float yStep = wallHeight / Mathf.Max(1, verticalStackCount - 1);
        float yStart = -wallHeight * 0.5f; // Center wall vertically around 0

        int particleIdx = 0;
        for (int i = 0; i < positions.Length; i++)
        {
            for (int y = 0; y < verticalStackCount; y++)
            {
                Vector3 pos = positions[i];
                pos.y = yStart + (y * yStep);

                particles[particleIdx].position = origin + pos;
                particles[particleIdx].startColor = particleColor;
                particles[particleIdx].startSize = particleSize;
                particles[particleIdx].remainingLifetime = 1000f;
                particles[particleIdx].velocity = Vector3.zero;

                particleIdx++;
            }
        }

        particleSys.SetParticles(particles, totalParticlesNeeded);
    }

    // 2. Generate Capsule Colliders stretched on the Y-axis for airflow collision
    private void UpdateCapsuleColliders(Vector3[] positions)
    {
        while (colliderPool.Count < positions.Length)
        {
            GameObject colObj = new GameObject($"WallColumn_{colliderPool.Count}");
            colObj.transform.SetParent(colliderHolder.transform, false);
            
            CapsuleCollider cc = colObj.AddComponent<CapsuleCollider>();
            cc.direction = 1; // 1 = Y-axis direction
            colliderPool.Add(cc);
        }

        Vector3 origin = transform.position;

        for (int i = 0; i < colliderPool.Count; i++)
        {
            if (i < positions.Length)
            {
                colliderPool[i].gameObject.SetActive(true);
                
                // Position capsule at center of the wall height
                colliderPool[i].transform.position = origin + positions[i];
                
                // Configure physical column dimensions
                colliderPool[i].height = wallHeight;
                colliderPool[i].radius = wallColliderRadius;
            }
            else
            {
                colliderPool[i].gameObject.SetActive(false);
            }
        }
    }

    private void SetupParticleSystem()
    {
        var main = particleSys.main;
        main.loop = false;
        main.playOnAwake = false;
        main.startSpeed = 0f;
        main.startLifetime = 1000f;
        main.maxParticles = 20000;
        main.simulationSpace = ParticleSystemSimulationSpace.World;

        var emission = particleSys.emission;
        emission.enabled = false;

        var shape = particleSys.shape;
        shape.enabled = false;

        var collision = particleSys.collision;
        collision.enabled = false;

        var renderer = GetComponent<ParticleSystemRenderer>();
        if (renderer.sharedMaterial == null)
        {
            Shader defaultShader = Shader.Find("Particles/Standard Unlit") ?? Shader.Find("Sprites/Default");
            if (defaultShader != null)
            {
                renderer.sharedMaterial = new Material(defaultShader);
            }
        }
    }

    private void OnDataReceived(IAsyncResult ar) 
    {
        try 
        {
            if (udpClient == null || udpClient.Client == null || !udpClient.Client.IsBound) return;

            IPEndPoint ip = new IPEndPoint(IPAddress.Any, port);
            byte[] bytes = udpClient.EndReceive(ar, ref ip);
            string json = Encoding.UTF8.GetString(bytes);

            ScanPacket packet = JsonUtility.FromJson<ScanPacket>(json);
            if (packet != null && packet.pts != null && packet.pts.Length > 0)
            {
                Vector3[] parsedPositions = new Vector3[packet.pts.Length];
                for (int i = 0; i < packet.pts.Length; i++)
                {
                    float rad = packet.pts[i].a * Mathf.Deg2Rad;
                    float dist = (packet.pts[i].d / 1000f) * distanceScale;
                    
                    parsedPositions[i] = new Vector3(Mathf.Sin(rad) * dist, 0f, Mathf.Cos(rad) * dist);
                }
                scanQueue.Enqueue(parsedPositions);
            }

            udpClient.BeginReceive(OnDataReceived, null);
        }
        catch (ObjectDisposedException) { }
        catch (Exception e)
        {
            Debug.LogError($"[LIDAR] Receive Error: {e.Message}");
        }
    }

    void OnDestroy() 
    {
        if (udpClient != null)
        {
            udpClient.Close();
            udpClient = null;
        }
    }
}