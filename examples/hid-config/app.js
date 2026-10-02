const VENDOR_ID = 0xcafe;
const PRODUCT_ID = 0x4004;

const REPORT_LAYOUT = 7;
const REPORT_SET_SLOT = 8;

const ACTION_SIZE = 16;

const ACTION_NONE = 0;
const ACTION_KEY = 1;
const ACTION_TEXT = 2;
const ACTION_LAUNCH = 3;
const ACTION_MACRO = 4;

const DEVICE_NONE = 0;
const DEVICE_BUTTON = 1;
const DEVICE_ENCODER = 2;

let device = null;
let selectedSlot = null;

const connectButton = document.querySelector("#connect");
const status = document.querySelector("#status");

const rowsInput = document.querySelector("#rows");
const colsInput = document.querySelector("#cols");
const grid = document.querySelector("#grid");

const slotEditor = document.querySelector("#slot-editor");
const slotRow = document.querySelector("#slot-row");
const slotCol = document.querySelector("#slot-col");

const deviceType = document.querySelector("#device-type");

const buttonEditor = document.querySelector("#button-editor");
const buttonPin = document.querySelector("#button-pin");
const buttonId = document.querySelector("#button-id");

const encoderEditor = document.querySelector("#encoder-editor");
const encoderPinA = document.querySelector("#encoder-pin-a");
const encoderPinB = document.querySelector("#encoder-pin-b");
const encoderPosition = document.querySelector("#encoder-position");
const encoderId = document.querySelector("#encoder-id");

const actionType = document.querySelector("#action-type");

const actionKey = document.querySelector("#action-key");
const actionText = document.querySelector("#action-text");
const actionLaunch = document.querySelector("#action-launch");
const actionMacro = document.querySelector("#action-macro");

const keyModifiers = document.querySelector("#key-modifiers");
const keyCode = document.querySelector("#key-code");

const textValue = document.querySelector("#text-value");

const launchApp = document.querySelector("#launch-app");

const macroId = document.querySelector("#macro-id");

// --------------------------------------------------
// HID
// --------------------------------------------------

connectButton.addEventListener("click", async () => {
  try {
    const devices = await navigator.hid.requestDevice({
      filters: [
        {
          vendorId: VENDOR_ID,
          productId: PRODUCT_ID,
        },
      ],
    });

    if (!devices.length) {
      return;
    }

    device = devices[0];

    if (!device.opened) {
      await device.open();
    }

    console.log("Connected device:", device);

    for (const collection of device.collections) {
      for (const report of collection.featureReports ?? []) {
        const size = report.items.reduce(
          (sum, item) => sum + (item.reportSize * item.reportCount) / 8,
          0,
        );

        console.log(`REPORT ${report.reportId}: ${size} bytes`, report.items);
      }
    }

    status.textContent = `Connected: ${device.productName || "HID device"}`;

    connectButton.textContent = "Connected";
  } catch (error) {
    console.error(error);
    status.textContent = "Connection failed";
  }
});

// --------------------------------------------------
// Layout
// --------------------------------------------------

document.querySelector("#set-layout").addEventListener("click", async () => {
  const rows = Number(rowsInput.value);
  const cols = Number(colsInput.value);

  if (!device) {
    alert("Connect the device first.");
    return;
  }

  if (rows < 1 || rows > 16 || cols < 1 || cols > 16) {
    alert("Rows and columns must be between 1 and 16.");
    return;
  }

  try {
    await sendLayout(rows, cols);
    createGrid(rows, cols);
  } catch (error) {
    console.error(error);
    alert(`Failed to create layout: ${error.message}`);
  }
});

async function sendLayout(rows, cols) {
  const data = new Uint8Array([rows, cols]);

  await device.sendFeatureReport(REPORT_LAYOUT, data);

  console.log(`Sent CREATE_LAYOUT: rows=${rows}, cols=${cols}`);
}

// --------------------------------------------------
// Grid
// --------------------------------------------------

function createGrid(rows, cols) {
  grid.innerHTML = "";

  grid.style.gridTemplateColumns = `repeat(${cols}, 1fr)`;

  for (let row = 0; row < rows; row++) {
    for (let col = 0; col < cols; col++) {
      const key = document.createElement("button");

      key.className = "key";

      key.innerHTML = `
        <strong>${row}, ${col}</strong>
        <small>empty</small>
      `;

      key.addEventListener("click", () => {
        selectSlot(row, col, key);
      });

      grid.appendChild(key);
    }
  }
}

// --------------------------------------------------
// Slot editor
// --------------------------------------------------

function selectSlot(row, col, element) {
  selectedSlot = {
    row,
    col,
    element,
  };

  slotRow.textContent = row;
  slotCol.textContent = col;

  slotEditor.hidden = false;

  // Reset device configuration
  deviceType.value = DEVICE_BUTTON;

  buttonPin.value = "";
  buttonId.value = "";

  encoderPinA.value = "";
  encoderPinB.value = "";
  encoderPosition.value = "0";
  encoderId.value = "";

  updateDeviceEditor();

  // Reset action
  actionType.value = ACTION_NONE;

  keyModifiers.value = 0;
  keyCode.value = 4;

  textValue.value = "";

  launchApp.value = 1;

  macroId.value = 0;

  updateActionEditor();
}

// --------------------------------------------------
// Device UI
// --------------------------------------------------

deviceType.addEventListener("change", updateDeviceEditor);

function updateDeviceEditor() {
  const type = Number(deviceType.value);

  buttonEditor.hidden = type !== DEVICE_BUTTON;

  encoderEditor.hidden = type !== DEVICE_ENCODER;
}

// --------------------------------------------------
// Action UI
// --------------------------------------------------

actionType.addEventListener("change", updateActionEditor);

function updateActionEditor() {
  actionKey.hidden = true;
  actionText.hidden = true;
  actionLaunch.hidden = true;
  actionMacro.hidden = true;

  switch (Number(actionType.value)) {
    case ACTION_KEY:
      actionKey.hidden = false;
      break;

    case ACTION_TEXT:
      actionText.hidden = false;
      break;

    case ACTION_LAUNCH:
      actionLaunch.hidden = false;
      break;

    case ACTION_MACRO:
      actionMacro.hidden = false;
      break;
  }
}

// --------------------------------------------------
// Set Slot
// --------------------------------------------------

document.querySelector("#set-slot").addEventListener("click", async () => {
  if (!device) {
    alert("Connect the device first.");
    return;
  }

  if (!selectedSlot) {
    return;
  }

  try {
    const row = selectedSlot.row;
    const col = selectedSlot.col;

    const changed = 0;
    const type = Number(deviceType.value);

    const action = encodeAction();
    const slotDevice = encodeDevice(type);

    await sendSlot(row, col, changed, type, action, slotDevice);

    selectedSlot.element.querySelector("small").textContent = describeAction();
  } catch (error) {
    console.error(error);
    alert(`Failed to set slot: ${error.message}`);
  }
});

// --------------------------------------------------
// Action serialization
//
// Action:
//
// byte 0     type
// byte 1     flags / modifiers
// byte 2     ID / key
// byte 3-15 data
//
// Total: 16 bytes
// --------------------------------------------------

function encodeAction() {
  const action = new Uint8Array(ACTION_SIZE);

  const type = Number(actionType.value);

  action[0] = type;

  switch (type) {
    case ACTION_NONE:
      break;

    case ACTION_KEY:
      action[1] = Number(keyModifiers.value);

      action[2] = Number(keyCode.value);

      break;

    case ACTION_TEXT: {
      const encoder = new TextEncoder();

      const encoded = encoder.encode(textValue.value);

      action.set(encoded.slice(0, ACTION_SIZE - 3), 3);

      break;
    }

    case ACTION_LAUNCH:
      action[2] = Number(launchApp.value);

      break;

    case ACTION_MACRO:
      action[2] = Number(macroId.value);

      break;

    default:
      throw new Error(`Unknown action type: ${type}`);
  }

  console.log(
    "Encoded action:",
    [...action].map((x) => x.toString(16).padStart(2, "0")).join(" "),
  );

  return action;
}

// --------------------------------------------------
// Device serialization
//
// DeviceSlot union:
//
// Button:
//
//     byte 0 = pin
//     byte 1 = id
//     byte 2 = pressed
//     byte 3-4 = padding
//
// Encoder:
//
//     byte 0 = pinA
//     byte 1 = pinB
//     byte 2 = last_b_position
//     byte 3 = id
//     byte 4 = percentage
//
// Union size = 5 bytes
// --------------------------------------------------

function encodeDevice(type) {
  const device = new Uint8Array(5);

  switch (type) {
    case DEVICE_NONE:
      break;

    case DEVICE_BUTTON:
      device[0] = Number(buttonPin.value);

      device[1] = Number(buttonId.value);

      device[2] = 0; // pressed

      break;

    case DEVICE_ENCODER:
      device[0] = Number(encoderPinA.value);

      device[1] = Number(encoderPinB.value);

      device[2] = 0; // last_b_position

      device[3] = Number(encoderId.value);

      device[4] = Number(encoderPosition.value);

      break;

    default:
      throw new Error(`Unknown device type: ${type}`);
  }

  console.log(
    "Encoded device:",
    [...device].map((x) => x.toString(16).padStart(2, "0")).join(" "),
  );

  return device;
}

// --------------------------------------------------
// Action description
// --------------------------------------------------

function describeAction() {
  switch (Number(actionType.value)) {
    case ACTION_NONE:
      return "empty";

    case ACTION_KEY:
      return `Key ${keyCode.value}`;

    case ACTION_TEXT:
      return `Text: ${textValue.value}`;

    case ACTION_LAUNCH:
      return launchApp.options[launchApp.selectedIndex].text;

    case ACTION_MACRO:
      return `Macro ${macroId.value}`;

    default:
      return "unknown";
  }
}

// --------------------------------------------------
// SET_SLOT
// --------------------------------------------------

async function sendSlot(row, col, changed, type, action, slotDevice) {
  /*
    DeviceSlot = 25 bytes

    4 bytes:
        row
        col
        changed
        type

    16 bytes:
        Action

    5 bytes:
        Button / Encoder union
  */

  const bytes = new Uint8Array(25);

  let offset = 0;

  // DeviceSlot header
  bytes[offset++] = row;
  bytes[offset++] = col;
  bytes[offset++] = changed;
  bytes[offset++] = type;

  // Action
  bytes.set(action, offset);

  offset += ACTION_SIZE;

  // Device union
  bytes.set(slotDevice, offset);

  console.log("SET_SLOT length:", bytes.length);

  console.log(
    "SET_SLOT payload:",
    [...bytes].map((x) => x.toString(16).padStart(2, "0")).join(" "),
  );

  if (bytes.length !== 25) {
    throw new Error(`Invalid SET_SLOT size: ${bytes.length}`);
  }

  await device.sendFeatureReport(REPORT_SET_SLOT, bytes);

  console.log(`Set slot ${row},${col}`);
}

// --------------------------------------------------
// Input reports
// --------------------------------------------------

navigator.hid.addEventListener("inputreport", (event) => {
  const { data, reportId } = event;

  const bytes = new Uint8Array(data.buffer, data.byteOffset, data.byteLength);

  console.log(
    `Received Interrupt Report ID ${reportId}:`,
    [...bytes].map((x) => x.toString(16).padStart(2, "0")).join(" "),
  );
});
