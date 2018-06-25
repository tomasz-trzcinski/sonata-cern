using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class ToggleTransparencyButtonScript : MonoBehaviour {

    private GameObject detector;
    void Start()
    {
        detector = GameObject.Find("aliceGeom");
        var btn = gameObject.GetComponent<Button>();
        btn.onClick.AddListener(TaskOnClick);
    }
    void TaskOnClick()
    {

        detector.GetComponent<detectorScript>().ToggleTransparency();
    }

}
