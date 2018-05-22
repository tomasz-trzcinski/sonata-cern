using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class detectorScript : MonoBehaviour
{
	private Transform[] transformList;
	private int iterator= 0;
	private Vector3 vector0 = new Vector3(0,0,0); 
	private Vector3 vector10 = new Vector3(10,10,10); 
	private bool doublePositions = true;
    // Use this for initialization
	void Start () {
		transformList = gameObject.GetComponentsInChildren<Transform> ();
		Debug.Log (transformList);
	}

	private void AddElement(Transform transform){
		if (transform.childCount > 1)
			foreach (Transform child in transform) {
				AddElement (child);
			}
		else {
			iterator++;
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
                        transform.position = Vector3.Scale(transform.position, new Vector3(2, 2, 2));
                    }
                    else
                    {
                        //if (transform.name == "EMCAL_XEN1_1")
                        //	transform.position = new Vector3 (0, 0, 0);
                        //else 
                        //if (transform.position.Equals (vector0)) 
                        //transform.localScale = Vector3.Scale(transform.localScale, new Vector3(0.5f,0.5f,0.5f));
                        //else
                        transform.position = Vector3.Scale(transform.position, new Vector3(0.5f, 0.5f, 0.5f));
                    }
                }
            }
            doublePositions = !doublePositions;
        
    }

    // Update is called once per frame
    void Update()
    {

        if (Input.GetKeyDown(KeyCode.Space))
            ToggleSize();

        //if (Input.GetKey(KeyCode.LeftArrow))
        //    transform.Rotate(Vector3.up * 50 * Time.deltaTime, Space.World);
        //if (Input.GetKey(KeyCode.RightArrow))
        //    transform.Rotate(Vector3.down * 50 * Time.deltaTime, Space.World);
        //if (Input.GetKey(KeyCode.UpArrow))
        //    transform.position -= transform.forward / 4 + new Vector3(Time.deltaTime, 0, 0);
        //if (Input.GetKey(KeyCode.DownArrow))
        //    transform.position += transform.forward / 4 + new Vector3(Time.deltaTime, 0, 0);

    }
}
