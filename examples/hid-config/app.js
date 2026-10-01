const VENDOR_ID = 0xcafe;
const PRODUCT_ID = 0x4004;

const REPORT_LAYOUT = 7;
const REPORT_SET_SLOT = 8;

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

const buttonPin = document.querySelector("#button-pin");
const buttonId = document.querySelector("#button-id");
const action = document.querySelector("#action");
const deviceType = document.querySelector("#device-type");

// --------------------------------------------------
// HID
// --------------------------------------------------

connectButton.addEventListener("click", async () => {
  try {
    const devices = await navigator.hid.requestDevice({
      filters: [],
    });

    console.log(devices);
    if (!devices.length) return;

    device = devices[0];

    if (!device.opened) await device.open();

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

  await sendLayout(rows, cols);

  createGrid(rows, cols);
});

async function sendLayout(rows, cols) {
  /*
        Python:

        report = bytes([7, rows, cols])

        WebHID:

        sendFeatureReport(reportId, data)
    */

  const data = new Uint8Array([rows, cols]);

  await device.sendFeatureReport(REPORT_LAYOUT, data);

  console.log(`Sent SET_LAYOUT: rows=${rows}, cols=${cols}`);
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

  buttonPin.value = "";
  buttonId.value = "";
  action.value = "";
}

document.querySelector("#set-slot").addEventListener("click", async () => {
  if (!device) {
    alert("Connect the device first.");
    return;
  }

  if (!selectedSlot) return;

  const row = selectedSlot.row;
  const col = selectedSlot.col;

  const changed = 0;
  const type = Number(deviceType.value);

  const pin = Number(buttonPin.value);
  const id = Number(buttonId.value);

  const actionText = action.value;

  await sendSlot(row, col, changed, type, actionText, pin, id);

  selectedSlot.element.querySelector("small").textContent =
    actionText || "empty";
});

// --------------------------------------------------
// SET_SLOT
// --------------------------------------------------

async function sendSlot(
  row,
  col,
  changed,
  deviceType,
  action,
  buttonPin,
  buttonId,
) {
  /*
        Your Python struct:

        <BBBB16s4s

        uint8_t row
        uint8_t col
        uint8_t changed
        uint8_t device_type

        char action[16]

        struct {
            uint8_t button_pin;
            uint8_t button_id;
            uint8_t button_pressed;
            uint8_t ???;
        }
    */

  const buffer = new ArrayBuffer(4 + 16 + 4);
  const view = new DataView(buffer);
  const bytes = new Uint8Array(buffer);

  let offset = 0;

  // <BBBB
  bytes[offset++] = row;
  bytes[offset++] = col;
  bytes[offset++] = changed;
  bytes[offset++] = deviceType;

  // 16-byte action
  const encoder = new TextEncoder();
  const actionBytes = encoder.encode(action);

  bytes.set(actionBytes.slice(0, 16), offset);

  offset += 16;

  // <4s button structure
  bytes[offset++] = buttonPin;
  bytes[offset++] = buttonId;
  bytes[offset++] = 0; // button_pressed
  bytes[offset++] = 0; // fourth byte

  console.log("SET_SLOT payload:", Array.from(bytes));

  await device.sendFeatureReport(REPORT_SET_SLOT, bytes);

  console.log(`Set slot ${row},${col}: ${action}`);
}
