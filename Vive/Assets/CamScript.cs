using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CamScript : MonoBehaviour {
    private Vector3 startPosition = new Vector3(14.3f, 0, 17.3f);
    private Quaternion startRotation = new Quaternion(0, -131.02f, 0,0);
    private Vector3 endPosition = new Vector3(22.35f, 0, 0);
    private Quaternion endRotation = new Quaternion(0, -90, 0,0);
    private bool sidePostion = false;
    public int speed=10;
	// Use this for initialization
	void Start () {

		
	}
	
	// Update is called once per frame
	void Update () {
        if (Input.GetKeyDown(KeyCode.C))
            sidePostion = !sidePostion;
        //float step = speed * Time.deltaTime;
        //transform.LookAt(new Vector3(0, 0, 0));
        if (sidePostion)
        {
           // transform.position = Vector3.MoveTowards(transform.position, endPosition, step);
        }
        else
        {
         //   transform.position = Vector3.MoveTowards(transform.position, startPosition, step);
        }
        //transform.LookAt(new Vector3(0, 0, 0));
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
