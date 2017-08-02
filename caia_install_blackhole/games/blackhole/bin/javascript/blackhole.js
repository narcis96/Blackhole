/**
 * Created by Ludo on 28-12-2016.
 */

/*******************************************************************************
 * BlackHoleModel is the model of the blackhole visualisation.
 * This part makes sure that the board transitions can be made.
 * When the board is changed, it sends the board to all its controllers.
 ******************************************************************************/

function BlackHoleModel() {
	this.char1 = '1'.charCodeAt(0);
	this.charA = 'A'.charCodeAt(0);

	this.size = 8;
	var N = this.size * (this.size - 1) / 2;
	this.owner = new Array(N); // falsish: empty, -1: hole, 1: player1, 2: player2
	this.number = new Array(N); // what number is on this tile?
	this.changeAt = new Array(N); // on which move was this position taken?

	this.curMove = 0;
	this.controllers = [];

	this.startPosition = 0;
	this.curPosition = 0;
}

BlackHoleModel.prototype.getSize = function() {
	return this.size;
};

BlackHoleModel.prototype.getPosition = function(move) {
	var row = move.charCodeAt(0) - this.charA;
	var col = move.charCodeAt(1) - this.char1;
	row += col;
	return row * (row + 1) / 2 + col;
}

BlackHoleModel.prototype.setHoles = function(holes) {
	this.holes = holes;
	for (var i in holes) {
		this.owner[this.getPosition(holes[i])] = -1;
	}
	return this;
};

BlackHoleModel.prototype.setMoves = function(moves) {
	this.moves = moves;
	for (var i in moves) {
		if (moves[i] == "Quit") continue;
		var p = this.getPosition(moves[i]);
		this.owner[p] = 1 + (i % 2);
		this.number[p] = Number(moves[i].slice(3));
		this.changeAt[p] = i;
	}
	return this;
};

BlackHoleModel.prototype.ownerAt = function(p, m) {
	return this.changeAt[p] >= m ? 0 : this.owner[p];
}

BlackHoleModel.prototype.addController = function(controller) {
	this.controllers.push(controller);
	if (controller.setMoves)
		controller.setMoves(this.moves);
	if (controller.onBoardChanged)
		controller.onBoardChanged(this.curMove);
};

BlackHoleModel.prototype.boardChanged = function() {
	// fire board change
	for (var i = this.controllers.length; i--;) {
		if (this.controllers[i].onBoardChanged)
			this.controllers[i].onBoardChanged(this.curMove);
	}
};

// returns whether the move has been processed
BlackHoleModel.prototype.setMove = function(move) {
	if (move < 0 || move > this.moves.length) return false;
	if (move == this.curMove) return true;

	this.curMove = move;
	this.boardChanged();
	return true;
};

/*******************************************************************************
 * BlackHoleController handles the interaction between the user and the BlackHoleModel
 ******************************************************************************/

function BlackHoleController(model, board, prev, next, playPause, moveTable) {
	this.sqrt3 = Math.sqrt(3);
	this.namesStone = [ "Player 1", "Player 2" ];
	this.model = model;

	this.board = document.getElementById(board);
	this.prev = document.getElementById(prev);
	this.next = document.getElementById(next);
	this.playPause = document.getElementById(playPause);
	this.moveTable = document.getElementById(moveTable);

	this.registerEvents();
	this.initCanvas();
}

BlackHoleController.prototype.setPlayerNames = function() {
	this.names = [];
	for (var i = 0; i < arguments.length; i++) {
		if ((typeof arguments[i]) == 'string') {
			this.names.push(arguments[i]);
		}
	}
	return this;
};

BlackHoleController.prototype.registerEvents = function() {
	var self = this, stopRunning = function() {
		self.isRunning = false;
		self.playPause.innerText = 'Play';
	};
	this.prev.onclick = function() {
		stopRunning();
		self.model.setMove(self.model.curMove - 1);
	};
	this.next.onclick = function() {
		stopRunning();
		self.model.setMove(self.model.curMove + 1);
	};
	this.playPause.onclick = function() {
		if (self.isRunning) {
			stopRunning();
		} else {
			self.isRunning = true;
			self.playPause.innerText = 'Pause';
			(function run() {
				if (self.isRunning) {
					if (!self.model.setMove(self.model.curMove + 1)) {
						// last move has been reached.
						stopRunning();
					} else {
						setTimeout(run, 750); // 4/3 moves per second
					}
				}
			})();
		}
	};
};


BlackHoleController.prototype.buildMoveTable = function(moves) {
	var table = document.createElement('table'), self = this;
	var theader = document.createElement('tr');
	theader.appendChild(document.createElement('th'));
	for (var i = 0; i < 2; i++) {
		var th = document.createElement('th');
		var playerName = this.namesStone[i];
		if (this.names && i < this.names.length) {
			// cut-off on more than 20 characters in the name
			var name = this.names[i];
			if (name.length > 20) name = name.slice(0, 17) + '...';
			playerName += ' (' + name + ')';
		}
		th.innerText = playerName;
		theader.appendChild(th);
	}

	var thead = document.createElement('thead');
	thead.appendChild(theader);
	table.appendChild(thead);

	var tbody = document.createElement('tbody');
	for (var i = 0; i <= moves.length; i++) {
		var tr = document.createElement('tr');
		var moveNr = document.createElement('th');
		moveNr.innerText = (i + 1) + '.';
		if (i % 2) tr.setAttribute('class', 'zebra');
		tr.appendChild(moveNr);

		var currentMove = i == 0 ? 'Start' : moves[i - 1];
		for (var j = 0; j < 2; j++) {
			var td = document.createElement('td');
			if (i == 0) { // start move
				if (j == 0) td.innerText = currentMove;
			} else if (currentMove == 'Quit' || (i % 2) != j) {
				td.innerText = currentMove;
			}
			tr.appendChild(td);
		}

		tr.onclick = (function(move) {
			return function() {
				self.model.setMove(move);
			}
		})(i);

		tbody.appendChild(tr);
	}

	this.updateSelectedRow = function(moveNr) {
		var lastCurMove = document.getElementById('cur-move');
		if (lastCurMove) {
			lastCurMove.removeAttribute('id');
		}
		if (moveNr >= 0) {
			var curRow = tbody.children[moveNr];
			curRow.setAttribute('id', 'cur-move');

			var topCoord = curRow.offsetTop;
			var bottomCoord = curRow.offsetTop + curRow.offsetHeight - self.moveTable.offsetHeight;
			if (self.moveTable.scrollTop < bottomCoord) {
				self.moveTable.scrollTop = bottomCoord;
			} else if (self.moveTable.scrollTop > topCoord) {
				self.moveTable.scrollTop = topCoord;
			}
		} else {
			self.moveTable.scrollTop = 0;
		}
	};
	this.updateSelectedRow(0);

	this.moveTable.setAttribute('tabindex', '1');
	this.moveTable.onkeydown = function(event) {
		if (event.keyCode == 38 || event.keyCode == 37) {
			self.model.setMove(self.model.curMove - 1);
		} else if (event.keyCode == 40 || event.keyCode == 39) {
			self.model.setMove(self.model.curMove + 1);
		} else {
			return;
		}
		event.preventDefault();
		event.stopImmediatePropagation();
	};

	table.appendChild(tbody);
	this.moveTable.appendChild(table);
};

BlackHoleController.prototype.initCanvas = function() {
	this.fillEmpty = [ "#EEEEEE", "#DDDDDD" ];
	this.fillHole = [ "#795548", "#694538" ];
	this.fillPlayer = [
		[ "#F44336", "#E43326" ],
		[ "#3F51B5", "#2F41A5" ]
	];
	this.textPlayer = [ "#222222", "#f8f8f8" ];
}

BlackHoleController.prototype.hexagonPath = function(context, topx, topy, tileSize, col1, col2) {
	context.beginPath();
	context.moveTo(topx, topy);
	context.lineTo(topx + 0.5 * tileSize, topy + tileSize * 0.5 / this.sqrt3);
	context.lineTo(topx + 0.5 * tileSize, topy + tileSize * 0.5 * this.sqrt3);
	context.lineTo(topx, topy + tileSize * 2.0 / this.sqrt3);
	context.lineTo(topx - 0.5 * tileSize, topy + tileSize * 0.5 * this.sqrt3);
	context.lineTo(topx - 0.5 * tileSize, topy + tileSize * 0.5 / this.sqrt3);
	context.closePath();

	if (col1 && col2) {
		var fill = context.createLinearGradient(topx, topy, topx, topy + tileSize * 0.5 / this.sqrt3);
		fill.addColorStop(0, col1);
		fill.addColorStop(1, col2);
		context.fillStyle = fill;
	}
}

BlackHoleController.prototype.drawBoard = function(move) {
	var context = this.board.getContext('2d');

	var height = Number(this.board.getAttribute("height"));
	var width  = Number(this.board.getAttribute("width" ));
	context.clearRect(0, 0, width, height);

	var padding = 10;
	var maxT1 = (height - padding) / (0.5 * this.sqrt3 * this.model.size + 1.0 / this.sqrt3); // based on height
	var maxT2 = (width  - padding) / this.model.size; // based on width

	var tileSize = Math.min(maxT1, maxT2);
	var boardTop = 0.5 * (height - tileSize * (0.5 * this.sqrt3 * this.model.size + 1.0 / this.sqrt3));

	// draw hexagons
	context.shadowBlur = context.shadowOffsetX = context.shadowOffsetY = 0;
	for (var row = 0; row < this.model.size; row++) {
		for (var col = 0; col <= row; col++) {
			var topx = 0.5 * (width + tileSize * (2 * col - row));
			var topy = boardTop + 0.5 * this.sqrt3 * tileSize * row;

			var i = row * (row + 1) / 2 + col;
			var owner = this.model.ownerAt(i, move);
			var colors = this.fillEmpty;
			if (owner == -1) colors = this.fillHole;
			if (owner > 0) colors = this.fillPlayer[owner - 1];

			this.hexagonPath(context, topx, topy, tileSize, colors[0], colors[1]);
			context.fill();
		}
	}

	// draw lines
	context.strokeStyle = 'rgba(40, 40, 40, 0.6)';
	context.lineWidth = 1.5;
	for (var row = 0; row < this.model.size; row++) {
		for (var col = 0; col <= row; col++) {
			var topx = 0.5 * (width + tileSize * (2 * col - row));
			var topy = boardTop + 0.5 * this.sqrt3 * tileSize * row;
			this.hexagonPath(context, topx, topy, tileSize);
			context.stroke();
		}
	}

	// draw text
	context.font = tileSize * 2.0 / (Math.sqrt(5) + 1) + "px Roboto, Helvetica, sans serif";
	context.textAlign = "center";
	context.textBaseline = "middle";
	context.shadowBlur = 1;
	context.shadowColor = "rgba(255, 255, 255, 0.2)";
	context.shadowOffsetX = 1;
	context.shadowOffsetY = 1;
	for (var row = 0; row < this.model.size; row++) {
		for (var col = 0; col <= row; col++) {
			var topx = 0.5 * (width + tileSize * (2 * col - row));
			var topy = boardTop + 0.5 * this.sqrt3 * tileSize * row;
/*
			context.font = "italic " + tileSize * 1.4 / (Math.sqrt(5) + 1) + "px Roboto";
			context.fillStyle = true ? "#222222" : "#f8f8f8";
			context.fillText(String.fromCharCode("A".charCodeAt(0) + row - col) + (col + 1), topx, topy + tileSize / this.sqrt3);
*/
			var i = row * (row + 1) / 2 + col;
			var owner = this.model.ownerAt(i, move);
			if (owner > 0) {
				// draw text
				context.fillStyle = this.textPlayer[owner - 1];
				context.fillText(this.model.number[i], topx, topy + tileSize / this.sqrt3);
			}
		}
	}
};

BlackHoleController.prototype.setMoves = function(moves) {
	this.buildMoveTable(moves);
};


BlackHoleController.prototype.onBoardChanged = function(move) {
	if (this.updateSelectedRow) {
		this.updateSelectedRow(move);
	}
	this.drawBoard(move);
};
