const width = window.innerWidth;
const height = window.innerHeight;

// Create SVG container
const svg = d3.select("#graph")
  .attr("width", width)
  .attr("height", height);

// Create a container group for all graph elements
const container = svg.append("g");

// Add zoom and pan behavior
svg.call(
  d3.zoom()
    .scaleExtent([0.5, 2]) // Set zoom scale limits
    .on("zoom", (event) => {
      container.attr("transform", event.transform); // Apply zoom and pan
    })
);

// Function to load and draw the graph
function loadGraph() {
  // Clear any existing graph elements
  container.selectAll("*").remove();

  // Load the graph data from the external JSON file
  d3.json("graphData.json").then(function(graphData) {

    // Create a map for quick look-up of nodes by ID
    const nodeMap = new Map();
    graphData.nodes.forEach(node => nodeMap.set(node.id, node));

    // Create an array for links based on the connections in the nodes
    const links = [];
    graphData.nodes.forEach(function(node) {
      node.connections.forEach(function(targetId) {
        let targetNode = nodeMap.get(targetId);

        // If target node doesn't exist, create a new node with gray color
        if (!targetNode) {
          targetNode = { id: targetId, color: "gray", connections: [] };
          graphData.nodes.push(targetNode); // Add the new node to the graph data
          nodeMap.set(targetId, targetNode); // Update the node map
        }

        // Create an undirected link between nodes (A <-> B)
        if (!links.some(link => (link.source === node.id && link.target === targetId) || (link.source === targetId && link.target === node.id))) {
          links.push({ source: node.id, target: targetId });
        }
      });
    });

    // Create a force simulation
    const simulation = d3.forceSimulation(graphData.nodes)
      .force("link", d3.forceLink(links).id(d => d.id).distance(150))
      .force("charge", d3.forceManyBody().strength(-800))
      .force("center", d3.forceCenter(width / 2, height / 2));

    // Create links
    const link = container.selectAll(".link")
      .data(links)
      .enter().append("line")
      .attr("class", "link")
      .attr("stroke", "#aaa")
      .attr("stroke-width", 2);

    // Create nodes
    const node = container.selectAll(".node")
      .data(graphData.nodes)
      .enter().append("g")
      .attr("class", "node")
      .call(d3.drag()
        .on("start", dragStart)
        .on("drag", dragging)
        .on("end", dragEnd)
      );

    // Create circle for each node
    node.append("circle")
      .attr("r", 20)
      .attr("fill", d => d.color || "steelblue"); // Use gray for missing nodes

    // Add text to the nodes
    node.append("text")
      .attr("dx", 25)
      .attr("dy", 5)
      .text(d => d.id);

    // Update positions of nodes and links based on the simulation
    simulation.on("tick", function() {
      link
        .attr("x1", d => d.source.x)
        .attr("y1", d => d.source.y)
        .attr("x2", d => d.target.x)
        .attr("y2", d => d.target.y);

      node
        .attr("transform", d => `translate(${d.x},${d.y})`);
    });

    // Drag functions
    function dragStart(event, d) {
      if (!event.active) simulation.alphaTarget(0.3).restart();
      d.fx = d.x;
      d.fy = d.y;
    }

    function dragging(event, d) {
      d.fx = event.x;
      d.fy = event.y;
    }

    function dragEnd(event, d) {
      if (!event.active) simulation.alphaTarget(0);
      d.fx = null;
      d.fy = null;
    }

  }).catch(function(error) {
    console.error("Error loading the graph data: ", error);
  });
}

// Initial graph load
loadGraph();

// Add event listener for reload button
document.getElementById("reloadButton").addEventListener("click", function() {
  loadGraph();  // Reload the graph data when the button is clicked
});

