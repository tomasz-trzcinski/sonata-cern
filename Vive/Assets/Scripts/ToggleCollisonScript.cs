using UnityEngine;
using UnityEngine.UI;
using System.Collections;

public class ToggleCollisonScript : MonoBehaviour
{
    private LineRenderer[] renderers;
    private int iterator =0;
    void Start()
    {
        renderers = GameObject.FindObjectsOfType<LineRenderer>();
        var btn = gameObject.GetComponent<Button>();
        btn.onClick.AddListener(TaskOnClick);
    }
    void TaskOnClick()
    {
        for(int i=0; i< renderers.Length; i++)
            
            renderers[i].enabled = !renderers[i].enabled;
    }

	void Update(){
	}

}