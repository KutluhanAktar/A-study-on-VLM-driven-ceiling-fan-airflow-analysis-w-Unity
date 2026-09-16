using System;
using System.Collections.Generic;
using UnityEngine;

public class LidarThreadDispatcher : MonoBehaviour
{
    private static readonly Queue<Action> ExecutionQueue = new Queue<Action>();
    private static LidarThreadDispatcher _instance;

    void Awake()
    {
        if (_instance == null) _instance = this;
    }

    void Update()
    {
        lock (ExecutionQueue)
        {
            while (ExecutionQueue.Count > 0)
            {
                ExecutionQueue.Dequeue()?.Invoke();
            }
        }
    }

    public static void ExecuteOnMainThread(Action action)
    {
        if (action == null) return;
        lock (ExecutionQueue)
        {
            ExecutionQueue.Enqueue(action);
        }
    }
}
