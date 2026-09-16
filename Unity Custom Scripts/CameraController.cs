using UnityEngine;
using UnityEngine.InputSystem;

public class CameraController : MonoBehaviour
{
    [Header("Movement Settings")]
    public float moveSpeed = 15.0f;
    public float shiftMultiplier = 2.5f;

    [Header("Zoom Settings")]
    public float zoomSpeed = 20.0f;
    public float minZoom = 2.0f;
    public float maxZoom = 80.0f;

    [Header("Elevation Settings")]
    public float verticalSpeed = 10.0f;

    private Camera cam;

    void Start()
    {
        cam = GetComponent<Camera>();
        if (cam == null)
        {
            cam = Camera.main;
        }
    }

    void Update()
    {
        if (Keyboard.current == null) return;

        HandleMovement();
        HandleZoom();
    }

    private void HandleMovement()
    {
        // Check Shift Key for speed boost
        bool isShiftPressed = Keyboard.current.leftShiftKey.isPressed;
        float currentSpeed = isShiftPressed ? moveSpeed * shiftMultiplier : moveSpeed;

        Vector3 moveDirection = Vector3.zero;

        // A / D or Left / Right Arrows (Pan Left / Right)
        if (Keyboard.current.aKey.isPressed || Keyboard.current.leftArrowKey.isPressed) moveDirection += Vector3.left;
        if (Keyboard.current.dKey.isPressed || Keyboard.current.rightArrowKey.isPressed) moveDirection += Vector3.right;

        // W / S or Up / Down Arrows (Pan Forward / Backward across world Z-axis)
        if (Keyboard.current.wKey.isPressed || Keyboard.current.upArrowKey.isPressed) moveDirection += Vector3.forward;
        if (Keyboard.current.sKey.isPressed || Keyboard.current.downArrowKey.isPressed) moveDirection += Vector3.back;

        // Apply movement relative to world coordinates so angle doesn't affect speed
        transform.position += moveDirection.normalized * currentSpeed * Time.deltaTime;

        // R / F (Elevation Up / Down across world Y-axis)
        if (Keyboard.current.rKey.isPressed)
        {
            transform.position += Vector3.up * verticalSpeed * Time.deltaTime;
        }
        if (Keyboard.current.fKey.isPressed)
        {
            transform.position += Vector3.down * verticalSpeed * Time.deltaTime;
        }
    }

    private void HandleZoom()
    {
        // Q (Zoom In) / E (Zoom Out)
        float zoomDelta = 0f;

        if (Keyboard.current.qKey.isPressed) zoomDelta -= 1f;
        if (Keyboard.current.eKey.isPressed) zoomDelta += 1f;

        if (cam.orthographic)
        {
            cam.orthographicSize += zoomDelta * zoomSpeed * Time.deltaTime;
            cam.orthographicSize = Mathf.Clamp(cam.orthographicSize, minZoom, maxZoom);
        }
        else
        {
            cam.fieldOfView += zoomDelta * zoomSpeed * Time.deltaTime;
            cam.fieldOfView = Mathf.Clamp(cam.fieldOfView, minZoom, maxZoom);
        }
    }
}