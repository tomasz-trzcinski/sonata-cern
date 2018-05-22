using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CamScript : MonoBehaviour {

	// Use this for initialization
	void Start () {
		
	}
	
	// Update is called once per frame
	void Update () {	

		if (Input.GetKey(KeyCode.LeftArrow))
			transform.Rotate(Vector3.up * 100 * Time.deltaTime, Space.World);
		if (Input.GetKey(KeyCode.RightArrow))
			transform.Rotate(Vector3.down * 100 * Time.deltaTime, Space.World);
		if (Input.GetKey(KeyCode.UpArrow))
			transform.position -= transform.forward / 4 + new Vector3(Time.deltaTime/2, 0, 0);
		if (Input.GetKey(KeyCode.DownArrow))
			transform.position += transform.forward / 4 + new Vector3(Time.deltaTime/2, 0, 0);
	}
}
