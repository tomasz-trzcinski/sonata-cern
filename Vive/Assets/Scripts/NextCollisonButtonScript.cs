using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class NextCollisonButtonScript : MonoBehaviour {
    
    private GameObject collisionParent;
    private LineScript lineScript;
    void Start()
    {
        collisionParent = GameObject.Find("Collision");
        lineScript = collisionParent.GetComponent<LineScript>();
        var btn = gameObject.GetComponent<Button>();
        btn.onClick.AddListener(TaskOnClick);
    }
    void TaskOnClick()
    {
        lineScript.RestartCollision();
    }

    void Update()
    {
    }
}
