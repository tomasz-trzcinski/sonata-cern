using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class StartCollisonScript : MonoBehaviour {

    void Start()
    {
        var btn = gameObject.GetComponent<Button>();
        if (btn)
            btn.onClick.AddListener(TaskOnClick);
    }
    void TaskOnClick()
    {
        GameObject[] collisionObjects = GameObject.FindGameObjectsWithTag("Collision");
        foreach (GameObject collisionObject in collisionObjects)
        {
            collisionObject.GetComponent<LineScript>().RestartCollision();
        }
    }
}
