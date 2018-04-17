using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class detectorScript : MonoBehaviour
{
    private bool visible = true;
    // Use this for initialization
    void Start () {
		
	}
    public void Hidden(bool isHidden)
    {
        //Renderer[] renderers = gameObject.GetComponentsInChildren<Renderer>();
        //foreach (Renderer r in renderers)
        //{
        //    r.enabled = !isHidden;
        //}
    }

    // Update is called once per frame
    void Update()
    {
        //if (Input.GetKeyDown(KeyCode.Space))
        //{
        //    visible = !visible;
        //    Hidden(visible);
        //}

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
