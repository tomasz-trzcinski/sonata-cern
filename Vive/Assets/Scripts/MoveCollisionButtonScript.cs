using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class MoveCollisionButtonScript : MonoBehaviour { 

    void Start()
    {
        var btn = gameObject.GetComponent<Button>();
        btn.onClick.AddListener(TaskOnClick);
    }
    void TaskOnClick()
    {
        LineScript[] lineScripts = GameObject.FindObjectsOfType<LineScript>();
        foreach (LineScript lineScript in lineScripts){
            lineScript.ToggleDoublePositions();
        }
    }

    void Update()
    {
    }
}
