using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class MenuScript : MonoBehaviour
{
    public GameObject menuPrefab;
    private GameObject menu;
    private bool showMenu = false;
    private Transform menuTransform;
    private SteamVR_TrackedObject trackedObj;

    private SteamVR_Controller.Device Controller
    {
        get { return SteamVR_Controller.Input((int)trackedObj.index); }
    }

    void Awake()
    {
        trackedObj = GetComponent<SteamVR_TrackedObject>();
    }
    // Use this for initialization
    void Start()
    {
        menu = Instantiate(menuPrefab);
        menuTransform = menu.transform;

    }
    private void UpdateMenuPostion()
    {
        menuTransform.position = trackedObj.transform.position;
        menuTransform.rotation = trackedObj.transform.rotation;
    }

    // Update is called once per frame
    void Update()
    {
        if (Controller.GetPressDown(SteamVR_Controller.ButtonMask.ApplicationMenu))
        {
            showMenu = !showMenu;
            if (showMenu)
            {
                menu.SetActive(true);
                UpdateMenuPostion();
            }
            else
                menu.SetActive(false);
        }
        if (showMenu)
            UpdateMenuPostion();
    }
}