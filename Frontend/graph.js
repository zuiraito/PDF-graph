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

// Define a maximum link length
const MAX_LINK_LENGTH = 200; // Example: Set maximum length to 200px

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
      .force("link", d3.forceLink(links)
        .id(d => d.id)
        .distance(link => {
          // Calculate a dynamic length but enforce a maximum limit
          const baseDistance = 100; // Default distance
          const targetConnections = link.target.connections?.length || 0;
          const sourceConnections = link.source.connections?.length || 0;
          const dynamicDistance = baseDistance + Math.min(targetConnections, sourceConnections) * 10;

          return Math.min(dynamicDistance, MAX_LINK_LENGTH); // Enforce maximum length
        }))
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

    // Create circles for each node with dynamic radius based on the number of connections
    node.append("circle")
      .attr("r", d => {
        const connectionCount = d.connections?.length || 0;
        return connectionCount > 0 ? 10 + connectionCount * 1.2 : 10;
      })
      .attr("fill", d => d.color || "teal");

    // Add text to the nodes
    node.append("text")
      .attr("dx", 25)
      .attr("dy", 5)
      .text(d => d.id);

    // Add click event for node selection
    node.on("click", (event, d) => {
      // Highlight the selected node and its connections
      node.selectAll("circle").attr("stroke", null).attr("fill", d => d.color || "teal");
      link.attr("stroke", "#aaa").attr("stroke-width", 2); // Reset all links

      // Highlight the selected node
      d3.select(event.currentTarget).select("circle").attr("fill", "red").attr("stroke-width", 3);

      // Highlight links connected to the selected node with increased intensity
      link.filter(l => l.source.id === d.id || l.target.id === d.id)
        .attr("stroke", "red").attr("stroke-width", 4);

      // Center the selected node by removing the global centering force and applying a custom attraction force
      updateForces(d);
    });

    // Function to update forces when a node is selected
    function updateForces(selectedNode) {
      simulation
        .force("center", null) // Remove global centering force
        .force("attractToNode", d3.forceManyBody()
          .strength((node) => (node.id === selectedNode.id ? -500 : -5))) // Strong attraction for the selected node
        .alpha(1) // Reset alpha to apply changes
        .restart();
    }

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

    // Deselect all nodes when clicking on the background
    svg.on("click", function(event) {
      if (event.target === svg.node()) {
        // Reset all node highlights and link styles
        node.selectAll("circle").attr("stroke", null).attr("fill", d => d.color || "gray");
        link.attr("stroke", "#aaa").attr("stroke-width", 2); // Reset all links
        simulation.force("center", d3.forceCenter(width / 2, height / 2)); // Reapply centering force
        simulation.alpha(1).restart(); // Restart simulation to re-center
      }
    });

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

