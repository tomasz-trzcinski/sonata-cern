using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class detectorScript : MonoBehaviour
{
	private Transform[] transformList;
	private List<Vector3> startPositions = new List<Vector3>();
	private List<Vector3> endPositions = new List<Vector3>();
    public Material[] detectorMaterials;
	private int speed = 1;
	private float ITS_displacement = 12f;
	private Vector3 vector0 = new Vector3(0,0,0); 
	private Vector3 vector10 = new Vector3(10,10,10); 
	private bool doublePositions = true;
    private bool isTransparent = false;

    // Use this for initialization
	void Start () {
		transformList = gameObject.GetComponentsInChildren<Transform> ();
		foreach (Transform transform in transformList) {
			startPositions.Add (transform.position);
			if (transform.name.StartsWith ("MAIN_GentleGeometry_ITS")) {
				endPositions.Add( transform.position + new Vector3(0,0,ITS_displacement));
				ITS_displacement -= 1.5f ;
				continue;
			}
			if (transform.name.StartsWith ("MAIN_GentleGeometry_TPC")) {
				endPositions.Add( transform.position + new Vector3(0,0,2));
				continue;
			}
			endPositions.Add (Vector3.Scale (transform.position, new Vector3 (2, 2, 2)));
			
		}
	}

    public void Hidden(bool isHidden)
    {
        //Renderer[] renderers = gameObject.GetComponentsInChildren<Renderer>();
        //foreach (Renderer r in renderers)
        //{
        //    r.enabled = !isHidden;
        //}
    }

    public void ToggleSize()
    {
            foreach (Transform transform in transformList)
            {
                if (transform.childCount < 1)
                {
                    if (doublePositions)
                    {
                        //if (transform.name == "EMCAL_XEN1_1")
                        //	transform.position = new Vector3 (5, 5, 5);
                        //else
                        //if (transform.position.Equals(vector0))
                        //	transform.localScale = Vector3.Scale(transform.localScale, new Vector3(2,2,2));
                        //else
					transform.position = Vector3.MoveTowards(transform.position,Vector3.Scale(transform.position, new Vector3(1, 1, 2)),10);
					if (transform.name.StartsWith ("MAIN_GentleGeometry_ITS")) {
						transform.position += new Vector3(0,0,ITS_displacement);
						ITS_displacement -= 0.5f;
					}
					if (transform.name.StartsWith ("MAIN_GentleGeometry_TPC")) {
						transform.position += new Vector3(0,0,2);
					}
					if (transform.name.StartsWith ("MAIN_GentleGeometry_TRD+TOF_BREF_1")) {
						transform.GetComponentInParent<Renderer>().enabled = false;
					}
                    }
                    else
                    {

					if (transform.name.StartsWith ("MAIN_GentleGeometry_ITS")) {
						transform.position -= new Vector3(0,0,ITS_displacement);
						ITS_displacement -= 0.5f;
					}
					if (transform.name.StartsWith ("MAIN_GentleGeometry_TPC")) {
						transform.position -= new Vector3(0,0,2);
					}
					if (transform.name.StartsWith ("MAIN_GentleGeometry_TRD+TOF_BREF_1")) {
						transform.GetComponentInParent<Renderer>().enabled = false;
					}
                        //if (transform.name == "EMCAL_XEN1_1")
                        //	transform.position = new Vector3 (0, 0, 0);
                        //else 
                        //if (transform.position.Equals (vector0)) 
                        //transform.localScale = Vector3.Scale(transform.localScale, new Vector3(0.5f,0.5f,0.5f));
                        //else
					transform.position =  Vector3.MoveTowards(transform.position,Vector3.Scale(transform.position, new Vector3(1, 1, 0.5f)),speed);
                    }
                }
            }
        
    }

    public void ToggleTransparency(){
        if (isTransparent)
        {
            foreach( Material material in detectorMaterials)
            {
                Color colorTemp = material.color;
                colorTemp.a = 1;
                material.color = colorTemp;
                StandardShaderUtils.ChangeRenderMode(material, StandardShaderUtils.BlendMode.Opaque);
            }
        }
        else
        {
            foreach (Material material in detectorMaterials)
            {
                Color colorTemp = material.color;
                colorTemp.a = 0.5f;
                material.color = colorTemp;
                StandardShaderUtils.ChangeRenderMode(material, StandardShaderUtils.BlendMode.Transparent);
            }
        }
        isTransparent = !isTransparent;
    }

    // Update is called once per frame
    void Update()
    {
		
		if (Input.GetKeyDown (KeyCode.Space))
			doublePositions = !doublePositions;
        if (Input.GetKeyDown(KeyCode.N))
            ToggleTransparency();
        float step = speed * Time.deltaTime;
		float ITS_displacement = 5f;
		int i = 0;
		foreach (Transform transform in transformList)
		{
			if (transform.childCount < 1)
			{
				if (!doublePositions)
				{
					transform.position = Vector3.MoveTowards(transform.position,endPositions[i],step);
					if (transform.name.StartsWith ("MAIN_GentleGeometry_TRD+TOF_BREF_1")) {
						transform.GetComponentInParent<Renderer>().enabled = false;
					}

				}
				else
				{
					if (transform.name.StartsWith ("MAIN_GentleGeometry_TRD+TOF_BREF_1")) {
						transform.GetComponentInParent<Renderer>().enabled = false;
					}
					transform.position =  Vector3.MoveTowards(transform.position, startPositions[i],step);
				}
			}
			i++;
		}

    }
}
