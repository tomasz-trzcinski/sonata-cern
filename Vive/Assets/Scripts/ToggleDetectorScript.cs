using UnityEngine;
using UnityEngine.UI;
using System.Collections;

public class ToggleDetectorScript : MonoBehaviour
{
    private Renderer[] renderers;
    private GameObject detector;
    void Start()
    {
        detector = GameObject.Find("aliceGeom");
        renderers = detector.GetComponentsInChildren<Renderer>();
        var btn = gameObject.GetComponent<Button>();
        btn.onClick.AddListener(TaskOnClick);
    }
    void TaskOnClick()
    {
        for(int i=0; i< renderers.Length; i++)
            renderers[i].enabled = !renderers[i].enabled;
        Debug.Log("You have clicked the button!");
    }


}