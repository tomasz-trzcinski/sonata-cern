using System.Collections.Generic;
using UnityEngine;
using System.Linq;
using UnityEditor;

public class Detectortoggle : MonoBehaviour {
    public static int layer = 31;
    private List<GameObject> objects;
    void GetObjectsInLayer(GameObject[] root, int layer)
    {
        List<GameObject> Selected = new List<GameObject>();
        foreach (GameObject t in root)
        {
            if (t.layer == layer)
            {
                Selected.Add(t);
            }
        }
        Selection.objects = Selected.ToArray();

    }
    static GameObject[] GetSceneObjects()
    {
        return Resources.FindObjectsOfTypeAll<GameObject>()
                .Where(go => go.hideFlags == HideFlags.None).ToArray();
    }
    // Use this for initialization
    void Start () {

        //objects = GetSceneObjects();
        //GetObjectsInLayer(objects, layer);
    }
	
	// Update is called once per frame
	void Update () {
		
	}
}
