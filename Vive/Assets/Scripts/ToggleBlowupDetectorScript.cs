using UnityEngine;
using UnityEngine.UI;
using System.Collections;

public class ToggleBlowupDetectorScript : MonoBehaviour
{
    private GameObject detector;
    void Start()
    {
        detector = GameObject.Find("aliceGeom");
        var btn = gameObject.GetComponent<Button>();
        btn.onClick.AddListener(TaskOnClick);
    }
    void TaskOnClick()
    {

        detector.GetComponent<detectorScript>().DoublePositions();
    }

}