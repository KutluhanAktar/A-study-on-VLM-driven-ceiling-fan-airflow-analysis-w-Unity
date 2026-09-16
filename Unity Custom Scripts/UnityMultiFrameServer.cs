using System;
using System.Collections;
using System.Collections.Generic;
using System.Net;
using System.Threading.Tasks;
using UnityEngine;

public class UnityMultiFrameServer : MonoBehaviour
{
    [Header("Server Configuration")]
    public int port = 8080;

    [Header("Capture Settings")]
    public Camera captureCamera;
    public int targetWidth = 640;
    public int targetHeight = 360;
    [Range(10, 100)] public int jpegQuality = 70;

    private HttpListener httpListener;
    private RenderTexture renderTexture;
    private Texture2D texture2D;
    private Rect readRect;

    // Recording task state
    private bool isRecordingRequested = false;
    private int requestedFrameCount = 72;
    private float requestedFps = 15f;
    private TaskCompletionSource<List<string>> recordingTaskSource;

    void Start()
    {
        if (captureCamera == null) captureCamera = Camera.main;

        renderTexture = new RenderTexture(targetWidth, targetHeight, 24, RenderTextureFormat.ARGB32);
        texture2D = new Texture2D(targetWidth, targetHeight, TextureFormat.RGB24, false);
        readRect = new Rect(0, 0, targetWidth, targetHeight);

        StartHttpServer();
    }

    private void StartHttpServer()
    {
        httpListener = new HttpListener();
        httpListener.Prefixes.Add($"http://*:{port}/get_video_frames/");
        httpListener.Start();
        Task.Run(ListenForRequests);
        Debug.Log($"Unity Video Server listening on http://localhost:{port}/get_video_frames/");
    }

    private async Task ListenForRequests()
    {
        while (httpListener != null && httpListener.IsListening)
        {
            try
            {
                var context = await httpListener.GetContextAsync();
                
                // Parse parameters from query: ?count=72&fps=15
                string countParam = context.Request.QueryString["count"];
                string fpsParam = context.Request.QueryString["fps"];

                if (!int.TryParse(countParam, out requestedFrameCount)) requestedFrameCount = 72;
                if (!float.TryParse(fpsParam, out requestedFps)) requestedFps = 15f;

                // Trigger Main-Thread sequence recording
                recordingTaskSource = new TaskCompletionSource<List<string>>();
                isRecordingRequested = true;

                // Wait until frame sequence finishes capturing
                List<string> base64Frames = await recordingTaskSource.Task;

                // Construct JSON response array
                string jsonResponse = "{\"frames\":[\"" + string.Join("\",\"", base64Frames) + "\"]}";
                byte[] buffer = System.Text.Encoding.UTF8.GetBytes(jsonResponse);

                context.Response.ContentType = "application/json";
                context.Response.ContentLength64 = buffer.Length;
                await context.Response.OutputStream.WriteAsync(buffer, 0, buffer.Length);
                context.Response.OutputStream.Close();
            }
            catch (ObjectDisposedException)
            {
                // Expected when httpListener.Stop() or Close() is called during shutdown
                break;
            }
            catch (HttpListenerException ex) when (ex.ErrorCode == 995) // ERROR_OPERATION_ABORTED
            {
                // Expected when listener stops while waiting
                break;
            }
            catch (Exception ex)
            {
                Debug.LogError($"HTTP Server Error: {ex.Message}");
            }
        }
    }

    void Update()
    {
        if (isRecordingRequested)
        {
            isRecordingRequested = false;
            StartCoroutine(CaptureFrameSequence(requestedFrameCount, requestedFps));
        }
    }

    private IEnumerator CaptureFrameSequence(int count, float fps)
    {
        List<string> frames = new List<string>(count);
        float interval = 1.0f / fps;

        for (int i = 0; i < count; i++)
        {
            yield return new WaitForEndOfFrame();

            // Render camera frame
            RenderTexture prevActive = RenderTexture.active;
            captureCamera.targetTexture = renderTexture;
            captureCamera.Render();

            RenderTexture.active = renderTexture;
            texture2D.ReadPixels(readRect, 0, 0);
            texture2D.Apply();

            captureCamera.targetTexture = null;
            RenderTexture.active = prevActive;

            // Encode to Base64 JPEG string
            byte[] jpegBytes = texture2D.EncodeToJPG(jpegQuality);
            string base64Str = Convert.ToBase64String(jpegBytes);
            frames.Add(base64Str);

            // Wait for interval before grabbing next frame
            yield return new WaitForSeconds(interval);
        }

        recordingTaskSource?.SetResult(frames);
    }

    private void OnDestroy()
    {
        if (httpListener != null)
        {
            if (httpListener.IsListening)
            {
                httpListener.Stop();
            }
            httpListener.Close();
            httpListener = null;
        }

        if (renderTexture != null)
        {
            renderTexture.Release();
            Destroy(renderTexture);
        }

        if (texture2D != null)
        {
            Destroy(texture2D);
        }
    }
}